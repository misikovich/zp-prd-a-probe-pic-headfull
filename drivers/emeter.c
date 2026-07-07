/*
 * MAX78615+PPM driver.
 *
 * Wire protocol (datasheet "Single Word SPI Reads/Writes", Tables 46-48):
 *   read : 0x01, ADDR[5:0]<<2,          3 dummy bytes clock out DATA msb 1st
 *   write: 0x01, (ADDR[5:0]<<2) | 0x02, DATA[23:16], DATA[15:8], DATA[7:0]
 * SPI mode 3 (CPOL = 1, CPHA = 1), MSB first, SCK <= 1 MHz, CS held low for
 * the whole 5-byte transaction. Registers are 24-bit words; single-word
 * transactions reach word addresses 0x00..0x3F only.
 * The shared SPI1_EM_CFG configuration provides the mode and clock.
 *
 * Liveness: CYCLE increments every high-rate sample (~3307 SPS, one tick
 * each ~302 us), so it must advance between the first and last read of a
 * poll sweep. FRAME increments per accumulation interval (default 1 s).
 *
 * Ported from the PIC24 test-probe project: raw GPIO/spi2_bus replaced
 * with MCC pins, the spi_bus module, and FreeRTOS bus arbitration (the
 * polled MCC driver has no TX/RX error paths, so those codes are gone).
 */

#include <libpic30.h>
#include <stddef.h>
#include <string.h>

#include "emeter.h"
#include "spi_bus.h"
#include "mcc_generated_files/system/clock.h"
#include "mcc_generated_files/system/pins.h"

#define EMETER_DELAY_CYCLES_PER_US (CLOCK_InstructionFrequencyGet() / 1000000UL)
#define EMETER_CS_SETTLE_US    1u
#define EMETER_ALIVE_WAIT_US   700u      /* > 2 CYCLE ticks at 3307 SPS */
/* upper bound on waiting for the bus; a running FPGA bitstream upload
   holds it for ~0.7 s, so a poll during an upload reports busy instead */
#define EMETER_BUS_TIMEOUT     pdMS_TO_TICKS(50)
#define EMETER_WORD_MASK       0x00FFFFFFUL
#define EMETER_WORD_ALL_ONES   0x00FFFFFFUL
#define EMETER_ADDR_MAX        0x3Fu
#define EMETER_SPI_START       0x01u
#define EMETER_SPI_OP_READ     0x00u
#define EMETER_SPI_OP_WRITE    0x02u
#define EMETER_FRAME_LEN       5u
/* nonzero upper bits of IND_WR_ADDR make the meter commit IND_WR_DATA
   to the target word at its next high-rate sample */
#define EMETER_IND_EXEC        0x000100UL
#define EMETER_RESET_CLEAR_MAX 3u
#define EMETER_SAMPLES_TRIES_MAX 2u
/* accumulation interval in high-rate samples: 331 of ~3307 SPS gives
   ~100 ms, i.e. ~10 fresh low-rate results per second (RAM copy only -
   never flash-saved, meter reverts to its stored 1 s default on reset) */
#define EMETER_SAMPLES_PER_INTERVAL 331UL

/* attempts spent on post-reboot housekeeping; re-armed once the RESET
   bit reads back clear, so a mid-session meter reboot is caught */
static u8 RESET_CLEAR_TRIES;
static u8 SAMPLES_TRIES;

static void emeter_delay_us(u16 us)
{
    while (us > 0u) {
        __delay32(EMETER_DELAY_CYCLES_PER_US);
        us--;
    }
}

/* one CS-framed full-duplex transfer, in place; bus claimed by the caller */
static void emeter_xfer(u8 *frame, u8 len)
{
    spi_bus_select(SPI_BUS_EMETER);
    emeter_delay_us(EMETER_CS_SETTLE_US);

    spi_bus_exchange(frame, len);

    emeter_delay_us(EMETER_CS_SETTLE_US);
    spi_bus_deselect(SPI_BUS_EMETER);
}

/* bus claimed by the caller */
static u8 emeter_read_reg_raw(u8 addr, u32 *value)
{
    u8 frame[EMETER_FRAME_LEN];

    if ((value == NULL) || (addr > EMETER_ADDR_MAX)) {
        return 0u;
    }

    frame[0] = EMETER_SPI_START;
    frame[1] = (u8)((u8)(addr << 2u) | EMETER_SPI_OP_READ);
    frame[2] = 0u;
    frame[3] = 0u;
    frame[4] = 0u;

    emeter_xfer(frame, EMETER_FRAME_LEN);

    *value = (((u32)frame[2] << 16u) | ((u32)frame[3] << 8u) |
            (u32)frame[4]) & EMETER_WORD_MASK;
    return 1u;
}

static u8 emeter_write_reg_raw(u8 addr, u32 value)
{
    u8 frame[EMETER_FRAME_LEN];

    if (addr > EMETER_ADDR_MAX) {
        return 0u;
    }

    frame[0] = EMETER_SPI_START;
    frame[1] = (u8)((u8)(addr << 2u) | EMETER_SPI_OP_WRITE);
    frame[2] = (u8)((value >> 16u) & 0xFFu);
    frame[3] = (u8)((value >> 8u) & 0xFFu);
    frame[4] = (u8)(value & 0xFFu);

    emeter_xfer(frame, EMETER_FRAME_LEN);
    return 1u;
}

/* queue an indirect write to a word address beyond the direct 0x3F
   window; the meter commits it within one high-rate sample (~302 us)
   and clears the upper bits of IND_WR_ADDR when done */
static u8 emeter_write_indirect_raw(u8 target, u32 value)
{
    if (!emeter_write_reg_raw(EMETER_REG_IND_WR_DATA, value)) {
        return 0u;
    }

    return emeter_write_reg_raw(EMETER_REG_IND_WR_ADDR,
            EMETER_IND_EXEC | (u32)target);
}

/* post-reboot housekeeping, driven by the sticky RESET status bit and
   paced at one indirect write per poll (the meter has a single indirect
   mailbox): first restore the fast accumulation interval the meter lost
   with its RAM, then clear the RESET bit. Attempts are capped so a chip
   that refuses the writes is not hammered forever and the RESET
   indication stays honest; observing the bit clear re-arms both steps
   for the next reboot */
static void emeter_reset_bit_housekeeping(const EmeterSample *sample)
{
    if ((sample->status & EMETER_STATUS_RESET) == 0UL) {
        RESET_CLEAR_TRIES = 0u;
        SAMPLES_TRIES = 0u;
        return;
    }

    if (SAMPLES_TRIES < EMETER_SAMPLES_TRIES_MAX) {
        SAMPLES_TRIES++;
        (void)emeter_write_indirect_raw(EMETER_XREG_SAMPLES,
                EMETER_SAMPLES_PER_INTERVAL);
        return;
    }

    if (RESET_CLEAR_TRIES < EMETER_RESET_CLEAR_MAX) {
        RESET_CLEAR_TRIES++;
        (void)emeter_write_indirect_raw(EMETER_XREG_STATUS_CLEAR,
                EMETER_STATUS_RESET);
    }
}

static void emeter_clear_sample(EmeterSample *sample)
{
    sample->link_ok = 0u;
    sample->alive = 0u;
    sample->busy = 0u;
    sample->error = EMETER_ERROR_NONE;
    sample->fw_version = 0UL;
    sample->status = 0UL;
    sample->cycle = 0UL;
    sample->frame = 0UL;
    sample->va_rms = 0UL;
    sample->vb_rms = 0UL;
    sample->vc_rms = 0UL;
    sample->pf_t = 0UL;
    sample->freq = 0UL;
    sample->tempc = 0UL;
}

static u8 emeter_sample_all_equal(const EmeterSample *sample, u32 word)
{
    return (sample->fw_version == word) &&
            (sample->status == word) &&
            (sample->cycle == word) &&
            (sample->frame == word) &&
            (sample->va_rms == word) &&
            (sample->vb_rms == word) &&
            (sample->vc_rms == word) &&
            (sample->pf_t == word) &&
            (sample->freq == word) &&
            (sample->tempc == word);
}

static u8 emeter_read_sweep(EmeterSample *sample, u32 *cycle_first)
{
    u8 ok;

    ok = emeter_read_reg_raw(EMETER_REG_CYCLE, cycle_first);
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_FW_VERSION,
            &sample->fw_version));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_STATUS, &sample->status));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_FRAME, &sample->frame));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_VA_RMS, &sample->va_rms));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_VB_RMS, &sample->vb_rms));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_VC_RMS, &sample->vc_rms));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_PF_T, &sample->pf_t));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_FREQ, &sample->freq));
    ok = (u8)(ok && emeter_read_reg_raw(EMETER_REG_TEMPC_A, &sample->tempc));

    if (ok) {
        emeter_delay_us(EMETER_ALIVE_WAIT_US);
        ok = emeter_read_reg_raw(EMETER_REG_CYCLE, &sample->cycle);
    }

    return ok;
}

void emeter_init(void)
{
    /* pins.c leaves the meter in reset; release it (active low) */
    EM_RST_SetHigh();
    EM_CS_SetHigh();

    RESET_CLEAR_TRIES = 0u;
    SAMPLES_TRIES = 0u;
}

u8 emeter_poll(EmeterSample *sample)
{
    u32 cycle_first;
    u8 ok;

    if (sample == NULL) {
        return 0u;
    }

    emeter_clear_sample(sample);

    if (!spi_bus_claim(EMETER_BUS_TIMEOUT)) {
        sample->busy = 1u;
        sample->error = EMETER_ERROR_BUS_BUSY;
        return 0u;
    }

    cycle_first = 0UL;
    ok = emeter_read_sweep(sample, &cycle_first);

    if (ok) {
        if (emeter_sample_all_equal(sample, 0UL)) {
            sample->error = EMETER_ERROR_ALL_ZERO;
            ok = 0u;
        } else if (emeter_sample_all_equal(sample, EMETER_WORD_ALL_ONES)) {
            sample->error = EMETER_ERROR_ALL_ONES;
            ok = 0u;
        } else if ((sample->fw_version == 0UL) ||
                (sample->fw_version == EMETER_WORD_ALL_ONES)) {
            sample->error = EMETER_ERROR_NO_LINK;
            ok = 0u;
        }
    }

    if (ok) {
        sample->link_ok = 1u;
        sample->alive = (sample->cycle != cycle_first) ? 1u : 0u;
        if (!sample->alive) {
            sample->error = EMETER_ERROR_STALE;
            ok = 0u;
        }
    }

    if (ok) {
        emeter_reset_bit_housekeeping(sample);
    }

    spi_bus_release();
    return ok;
}

u8 emeter_read_reg(u8 addr, u32 *value)
{
    u8 ok;

    if (!spi_bus_claim(EMETER_BUS_TIMEOUT)) {
        return 0u;
    }

    ok = emeter_read_reg_raw(addr, value);
    spi_bus_release();
    return ok;
}

u8 emeter_write_reg(u8 addr, u32 value)
{
    u8 ok;

    if (!spi_bus_claim(EMETER_BUS_TIMEOUT)) {
        return 0u;
    }

    ok = emeter_write_reg_raw(addr, value);
    spi_bus_release();
    return ok;
}

s32 emeter_s24(u32 raw)
{
    raw &= EMETER_WORD_MASK;
    if ((raw & 0x00800000UL) != 0UL) {
        return (s32)(raw | 0xFF000000UL);
    }

    return (s32)raw;
}
