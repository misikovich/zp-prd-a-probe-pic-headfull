/*
 * iCE5 FPGA bitstream upload over the shared SPI1 bus.
 *
 * Ported from the PIC24 test-probe project: raw GPIO/spi2_bus replaced
 * with MCC pins, the spi_bus module, and FreeRTOS bus arbitration.  The
 * bus is claimed for the whole upload, so emeter traffic simply waits
 * (its short claim timeout reports busy). The bus service selects the
 * FPGA-specific SPI configuration for the claim.
 */

#include <libpic30.h>
#include <stddef.h>
#include <xc.h>

#include "fpga.h"
#include "fpga_bitstream.h"
#include "spi_bus.h"
#include "mcc_generated_files/system/clock.h"
#include "mcc_generated_files/system/pins.h"

#define FPGA_DELAY_CYCLES_PER_US     (CLOCK_InstructionFrequencyGet() / 1000000UL)
#define FPGA_CRESET_LOW_US           10u
#define FPGA_CLEAR_WAIT_US           2000u
#define FPGA_POST_CONFIG_BYTES       7u
#define FPGA_CDONE_TIMEOUT_US        5000u
#define FPGA_BITSTREAM_SYNC_OFFSET   81u
/* whole-upload bus claim: generous, an emeter poll sweep takes ~1 ms */
#define FPGA_BUS_TIMEOUT             pdMS_TO_TICKS(500)

static void fpga_delay_us(u16 us)
{
    while (us > 0u) {
        __delay32(FPGA_DELAY_CYCLES_PER_US);
        us--;
    }
}

/* An unconfigured iCE5 with CRESET released and CS high enters SPI
   *master* configuration mode and drives the shared SCK/data lines,
   corrupting eMeter traffic. Park it in reset until a bitstream upload
   actually runs. */
void fpga_park(void)
{
    FPGA_CRST_SetLow();
}

/* reader over the RLE-packed flash chunks (an escape sequence may span a
   chunk boundary, so chunk advance lives in the byte fetch) */
typedef struct {
    u16 chunk;
    u16 pos;
    u32 consumed;
} FpgaPackedCursor;

static u8 fpga_packed_next(FpgaPackedCursor *cur)
{
    while (cur->pos >= FPGA_BITSTREAM_CHUNK_LEN[cur->chunk]) {
        cur->chunk++;
        cur->pos = 0u;
    }

    cur->consumed++;
    return FPGA_BITSTREAM_CHUNKS[cur->chunk][cur->pos++];
}

/* emit one decoded bitstream byte; the first SYNC_OFFSET bytes are the
   human-readable image header the FPGA must not see */
static spi_bus_result_t fpga_raw_emit(u32 *raw_pos, u8 byte)
{
    spi_bus_result_t result;

    result = SPI_BUS_OK;
    if (*raw_pos >= FPGA_BITSTREAM_SYNC_OFFSET) {
        result = spi_bus_exchange_byte(byte, NULL);
    }
    (*raw_pos)++;
    return result;
}

/* decode the RLE stream (see tools/fpga_bitstream_pack.py) straight into
   the SPI byte loop; no RAM buffer between flash and the bus */
static spi_bus_result_t fpga_stream_bitstream(void)
{
    FpgaPackedCursor cur = { 0u, 0u, 0UL };
    u32 packed_len;
    u32 raw_pos;
    u16 chunk;
    u8 byte;
    u8 value;
    u8 count;
    spi_bus_result_t result;

    packed_len = 0UL;
    for (chunk = 0u; chunk < FPGA_BITSTREAM_CHUNK_COUNT; chunk++) {
        packed_len += FPGA_BITSTREAM_CHUNK_LEN[chunk];
    }

    raw_pos = 0UL;
    while (cur.consumed < packed_len) {
        byte = fpga_packed_next(&cur);

        if (byte != FPGA_BITSTREAM_RLE_ESC) {
            result = fpga_raw_emit(&raw_pos, byte);
            if (result != SPI_BUS_OK) {
                return result;
            }
            continue;
        }

        value = fpga_packed_next(&cur);
        if (value == FPGA_BITSTREAM_RLE_ESC) {
            result = fpga_raw_emit(&raw_pos, value);
            if (result != SPI_BUS_OK) {
                return result;
            }
            continue;
        }

        count = fpga_packed_next(&cur);
        while (count > 0u) {
            result = fpga_raw_emit(&raw_pos, value);
            if (result != SPI_BUS_OK) {
                return result;
            }
            count--;
        }
    }

    return SPI_BUS_OK;
}

static spi_bus_result_t fpga_post_config_clocks(void)
{
    u8 i;
    spi_bus_result_t result;

    for (i = 0u; i < FPGA_POST_CONFIG_BYTES; i++) {
        result = spi_bus_exchange_byte(0x00u, NULL);
        if (result != SPI_BUS_OK) {
            return result;
        }
    }

    return SPI_BUS_OK;
}

static u8 fpga_wait_cdone(void)
{
    u16 elapsed_us;

    for (elapsed_us = 0u; elapsed_us < FPGA_CDONE_TIMEOUT_US; elapsed_us++) {
        if (FPGA_CDONE_GetValue()) {
            return 1u;
        }
        fpga_delay_us(1u);
    }

    return FPGA_CDONE_GetValue();
}

static volatile bool ICE5_CDONE = 0u;
static volatile bool ICE5_UPLOADING = 0u;
static volatile bool ICE5_UPLOAD_FAILED = 0u;
static volatile u8 ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_IDLE;

void fpga_prog_load(void)
{
    spi_bus_result_t result;
    u8 ok;

    if (ICE5_UPLOADING) {
        return;
    }

    ok = 0u;
    ICE5_CDONE = 0u;
    ICE5_UPLOAD_FAILED = 0u;
    ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_LOADING;
    ICE5_UPLOADING = 1u;

    /* release the user-logic reset; CRESET stays under our control */
    FPGA_RST_SetHigh();

    result = spi_bus_claim(SPI_BUS_FPGA, FPGA_BUS_TIMEOUT);
    if (result != SPI_BUS_OK) {
        ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_BUS_FAILED;
        FPGA_CRST_SetLow();
        ICE5_UPLOAD_FAILED = 1u;
        ICE5_UPLOADING = 0u;
        return;
    }

    /* CS low across the CRESET pulse selects SPI-slave configuration */
    result = spi_bus_select();
    if (result != SPI_BUS_OK) {
        ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_BUS_FAILED;
        goto fpga_prog_done;
    }
    FPGA_CRST_SetLow();
    fpga_delay_us(FPGA_CRESET_LOW_US);
    FPGA_CRST_SetHigh();
    fpga_delay_us(FPGA_CLEAR_WAIT_US);

    result = fpga_stream_bitstream();
    if (result != SPI_BUS_OK) {
        ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_BUS_FAILED;
        goto fpga_prog_done;
    }

    ICE5_CDONE = fpga_wait_cdone();
    if (ICE5_CDONE) {
        result = fpga_post_config_clocks();
        if (result == SPI_BUS_OK) {
            ok = 1u;
        } else {
            ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_BUS_FAILED;
        }
    } else {
        ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_CDONE_FAILED;
    }

fpga_prog_done:
    if (!ok) {
        /* unconfigured FPGA: hold it in reset, off the shared bus */
        FPGA_CRST_SetLow();
    }
    spi_bus_release();

    ICE5_UPLOAD_FAILED = ok ? 0u : 1u;
    if (ok) {
        ICE5_UPLOAD_STATUS = FPGA_UPLOAD_STATUS_DONE;
    }
    ICE5_UPLOADING = 0u;
}

bool fpga_is_cdone(void)
{
    return 1 == ICE5_CDONE;
}

bool fpga_is_uploading(void)
{
    return 1 == ICE5_UPLOADING;
}

bool fpga_upload_failed(void)
{
    return 1 == ICE5_UPLOAD_FAILED;
}

u8 fpga_upload_status(void)
{
    return ICE5_UPLOAD_STATUS;
}
