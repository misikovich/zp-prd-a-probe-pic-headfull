#ifndef WPROTOCOL_H
#define WPROTOCOL_H

#include <stdint.h>

/*
    WIRELESS DATA TRANSFER PROTOCOL (v1)

    Product A has no integrated screen, so captured diagnosis values are
    shared over a wireless link:

        PIC <-*UART*-> ESP <-*BT*-> Client Diagnosis Device

    The ESP forwards client -> PIC bytes untouched. In the PIC -> client
    direction it parses the stream: valid frames whose HDR source is
    WP_SRC_PICESP are addressed to the ESP itself (sound commands); the
    ESP consumes them and they never appear on the BT link. Every other
    byte — other frames, and bytes that fail frame parsing (bad CRC,
    interbyte timeout, garbage) — is forwarded to BT exactly once, so
    the bridge stays transparent and the client parser resynchronizes
    with its usual rules. The ESP also injects WP_TYPE_CONNECTED frames
    (src WP_SRC_CLIENT) into the UART stream to tell the PIC when the
    BT client session opens or closes; the PIC reacts by commanding the
    connect/disconnect sounds back (the ESP never acts on its own).

    FRAME FORMAT
    ------------
        SOF1 SOF2 | HDR | TYPE | LEN | VALUE[LEN] | CRC_L CRC_H

    SOF1 SOF2   Start of frame, always 0xA5 0x5A. Two bytes make false
                sync on payload data unlikely (1/65536) without needing
                byte escaping.
    HDR         Bit 7..4: protocol version (currently 1).
                Bit 3..0: source, WP_SRC_* (direction of travel).
    TYPE        Payload type, one of wp_type_t. 255 usable types;
                0x00 is reserved/invalid.
    LEN         Number of VALUE bytes, 0..255.
    VALUE       Payload. All multi-byte fields are little-endian.
                Floats are avoided; scaled fixed-point integers are used
                instead (scale documented per type below).
    CRC         CRC-16/CCITT-FALSE (poly 0x1021, init 0xFFFF, no reflect,
                no final XOR) computed over HDR, TYPE, LEN and VALUE
                (SOF excluded). Transmitted little-endian: CRC_L first.

    RECEIVER RULES
    --------------
    - Sync by scanning the byte stream for SOF1 SOF2.
    - On CRC mismatch, discard the frame and resume scanning one byte
      after the SOF1 that started it (handles false sync on payload).
    - If more than WP_INTERBYTE_TIMEOUT_MS elapses between bytes of a
      partial frame, reset the parser and rescan.
    - Unknown TYPE with a valid CRC: skip the frame silently (LEN makes
      every frame self-delimiting, so old firmware tolerates new types).
    - A frame with a mismatched protocol version is dropped; the receiver
      may report WP_TYPE_ERROR back once per session.

    RELIABILITY
    -----------
    The link is fire-and-forget for periodic measurements (a lost sample
    is superseded by the next one).

    ACT types (WP_ACT_*) are both command and state. The probe reports
    each ACT value periodically like any other measurement (1 = test
    active, 0 = idle). Unless documented otherwise, the client keeps
    sending the desired ACT value until the reported state converges
    (level-triggered and idempotent, so lost frames need no ACK and
    duplicates are harmless). WP_ACT_FPGA_TEST is a one-shot exception:
    command 1 queues one upload, command 0 is ignored, and duplicate 1
    commands are ignored while an upload is queued or loading. Its
    reported state is 1 while queued/loading and auto-clears to 0.

    WP_TYPE_ACK is reserved for future frames that have no observable
    state to converge on (e.g. one-shot config): the receiver echoes
    WP_TYPE_ACK carrying the TYPE byte it accepted; the sender retries
    up to 3 times on a WP_ACK_TIMEOUT_MS timeout.

    EXAMPLE
    -------
        A5 5A [11 01 02 D2 08] xx xx
              CRC Computable

        A5 5A  - SOF
        11     - version 1, source: probe
        01     - TYPE:  WP_TYPE_AMBIENT_TEMP
        02     - LEN:   2 bytes
        D2 08  - VALUE: 0x08D2 = 2258 -> 22.58 degC (int16, 0.01 degC/LSB)
        xx xx  - CRC-16 over bytes 11 01 02 D2 08, low byte first
*/

#define WP_SOF1                 0xA5u
#define WP_SOF2                 0x5Au
#define WP_HEARTBEAT_VALUE      0xCCu

#define WP_VERSION              1u

/* HDR = (WP_VERSION << 4) | WP_SRC_x */
#define WP_SRC_PROBE            0x1u    /* PIC -> client */
#define WP_SRC_CLIENT           0x2u    /* PIC <- client */
#define WP_SRC_ESPPIC           0x3u    /* ESP -> PIC, not transmitted */
#define WP_SRC_PICESP           0x4u    /* ESP <- PIC, not transmitted */

#define WP_MAX_VALUE_LEN        255u
#define WP_OVERHEAD_LEN         7u      /* SOF(2) + HDR + TYPE + LEN + CRC(2) */
#define WP_MAX_FRAME_LEN        (WP_OVERHEAD_LEN + WP_MAX_VALUE_LEN)
#define WP_BUFFER_LOG_SIZE      5u      /* capture 5 samples per frame for graphed values */

#define WP_CRC_INIT             0xFFFFu
#define WP_CRC_POLY             0x1021u

#define WP_INTERBYTE_TIMEOUT_MS 50u
#define WP_ACK_TIMEOUT_MS       200u
#define WP_ACK_RETRIES          3u

/* WP_TYPE_FPGA_STATUS payload values. */
#define WP_FPGA_STATUS_IDLE         0u
#define WP_FPGA_STATUS_LOADING      1u
#define WP_FPGA_STATUS_DONE         2u
#define WP_FPGA_STATUS_BUS_FAILED   3u
#define WP_FPGA_STATUS_CDONE_FAILED 4u

/* WP_TYPE_RCD_STATUS payload values. */
#define WP_RCD_STATUS_IDLE             0u
#define WP_RCD_STATUS_RUNNING          1u
#define WP_RCD_STATUS_PASS             2u
#define WP_RCD_STATUS_INITIAL_FAULT    3u
#define WP_RCD_STATUS_F6_TIMEOUT       4u
#define WP_RCD_STATUS_F30_TIMEOUT      5u
#define WP_RCD_STATUS_RECOVERY_TIMEOUT 6u
#define WP_RCD_STATUS_SENSOR_ERROR     7u
#define WP_RCD_STATUS_SEQUENCE_FAILED  8u

typedef enum {
  WP_TYPE_INVALID      = 0x00, /* reserved, never transmitted */

  /* measurements, probe -> client, fire-and-forget */
  /* Activateable - ACT command can be sent to perform a test action */

  /* Generic Sensors */
  WP_TYPE_TEMP_AMBIENT_ADC  = 0x01, /* uint16, 0-65535 */
  WP_TYPE_TEMP_SOCKET_ADC   = 0x02, /* uint16, 0-65535 */
  WP_TYPE_TEMP_RCD_ADC      = 0x03, /* uint16, 0-65535 */
  WP_TYPE_PWR_HWID_ADC      = 0x04, /* uint16, 0-65535 */

  /* MAX78615+PPM/C01 EMeter */
  WP_TYPE_EM_LINK           = 0x10, /* uint8, states: 00 - all zero, FF - all F, 01 - ok */
  WP_TYPE_EM_VA             = 0x11, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_VB             = 0x12, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_VC             = 0x13, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IA             = 0x14, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IB             = 0x15, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IMAX           = 0x16, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_FWVER          = 0x17, /* uint8[3], 3 bytes 04FC08 */
  WP_TYPE_EM_DRDY           = 0x18, /* uint8, 0-1 bool */
  WP_TYPE_EM_FHZ            = 0x19, /* uint16, 0.01 hz/LSB */
  WP_TYPE_EM_WARN           = 0x1A, /* uint8 */
  WP_TYPE_EM_ERR            = 0x1B, /* uint8 */
  WP_TYPE_EM_TEMP           = 0x1C, /* int16, die temp, 0.01 degC/LSB */
  WP_TYPE_EM_INTERRUPT      = 0x1D, /* uint8, IRQ pin level, active low (0 = asserted) */
  WP_TYPE_EM_ALARM          = 0x1E, /* uint8, alarm pin level, active low (0 = asserted) */

  /* Motor/Servo - Activateable */
  WP_ACT_SRV_TEST           = 0x20, /* uint8, 0-1 bool */
  WP_TYPE_SRV_POS_ADC       = 0x21, /* uint16[buffer_log_size], array */
  WP_TYPE_SRV_CURR_ADC      = 0x22, /* uint16[buffer_log_size], array */
  WP_TYPE_SRV_STATE         = 0x23, /* uint8, states: FF - stop, 01 - locking, 02 - unlocking */

  /* Grid Detect Voltages*/
  WP_TYPE_GRID_NGND_ADC     = 0x31, /* uint16, 0-65535 */
  WP_TYPE_GRID_L1L2_ADC     = 0x32, /* uint16, 0-65535 */

  /* benvac RCMB121-1 RCD Testing - Activateable */
  WP_ACT_RCD_TEST           = 0x40, /* uint8: command 1 queues one self-test;
                                       0 ignored; state 1 queued/running */
  WP_TYPE_RCD_ERR           = 0x41, /* uint8, logical error assertion */
  WP_TYPE_RCD_F6MA          = 0x42, /* uint8, raw pin: 1 = trip asserted */
  WP_TYPE_RCD_F30MA         = 0x43, /* uint8, raw pin: 1 = trip asserted */
  WP_TYPE_RCD_STATUS        = 0x44, /* uint8: WP_RCD_STATUS_* */

  /* iCE5LP1K-SG48 FPGA/Relay Testing - Activateable */
  WP_ACT_FPGA_TEST          = 0x50, /* uint8: command 1 queues one upload;
                                       0 ignored; state 1 queued/loading */
  WP_TYPE_FPGA_CDONE        = 0x51, /* uint8, 0-1 bool */
  WP_TYPE_FPGA_INT          = 0x52, /* uint8, 0-1 bool */
  WP_TYPE_FPGA_STATUS       = 0x53, /* uint8: 0 IDLE, 1 LOADING, 2 DONE,
                                       3 BUS_FAILED, 4 CDONE_FAILED */

  
  /* control, acknowledged */
  WP_TYPE_ACK          = 0xF0, /* uint8: TYPE being acknowledged */
  WP_TYPE_ERROR        = 0xF1, /* uint8: error code */
  WP_TYPE_HEARTBEAT    = 0xFF, /* uint8: CC */

  /* Internal ESP -> PIC status, sent over UART with src WP_SRC_CLIENT so
     the PIC parses one inbound source; never appears on the BT link.
     Fire-and-forget: the UART link is wired, no ACK. */
  WP_TYPE_CONNECTED       = 0xE0, /* uint8, 0-1 bool: BT client session
                                     open; sent on connect/disconnect */

  /* Internal PIC-ESP communication (src WP_SRC_PICESP), consumed by the
     ESP and never forwarded over BT. LEN = 0: the TYPE byte alone is the
     command; the ESP ignores VALUE, so a payload can be added later
     without breaking older ESP firmware. Fire-and-forget, no ACK. */
  INT_ACT_SOUND_GENERIC   = 0xE1, /* single generic beep */
  INT_ACT_SOUND_DOUBLE    = 0xE2, /* double generic beep */
  INT_ACT_SOUND_TRIPLE    = 0xE3, /* triple generic beep */
  INT_ACT_SOUND_SWITCH    = 0xE4, /* page-switch tick */
  INT_ACT_SOUND_OK        = 0xE5, /* ok/confirm beep */
  INT_ACT_SOUND_ERR       = 0xE6, /* error beep */
  INT_ACT_SOUND_STARTUP   = 0xE7, /* startup melody */
  INT_ACT_SOUND_CONNECT   = 0xE8, /* rising two-tone; PIC typically sends
                                     it on WP_TYPE_CONNECTED = 1 */
  INT_ACT_SOUND_DISCONNECT = 0xE9, /* falling two-tone; PIC typically sends
                                      it on WP_TYPE_CONNECTED = 0 */

} wp_type_t;

/* CRC-16/CCITT-FALSE over HDR, TYPE, LEN and VALUE (see FRAME FORMAT).
   Implemented in wprotocol.c, shared by the PIC and ESP builds. */
uint16_t wp_crc16(const uint8_t *data, uint16_t len);

/* Build a v1 frame into out, which must hold WP_OVERHEAD_LEN + len bytes.
   value may be NULL when len is 0. Returns the total frame length. */
uint16_t wp_frame_build(uint8_t *out, uint8_t src, uint8_t type,
                        const uint8_t *value, uint8_t len);

/* Streaming receiver implementing the RECEIVER RULES above for an end
   receiver (invalid bytes are discarded; the ESP bridge keeps its own
   forwarding filter). Feed bytes one at a time; when wp_rx_feed returns
   true, frame[] holds a complete CRC-valid frame:

       frame[2] = HDR, frame[3] = TYPE, frame[4] = LEN, &frame[5] = VALUE

   The frame is consumed by the next feed. On CRC mismatch the receiver
   resynchronizes on the next SOF inside the bytes already collected, so
   a real frame that started inside a false one is recovered. The caller
   owns the interbyte timeout: call wp_rx_reset when more than
   WP_INTERBYTE_TIMEOUT_MS passes mid-frame. */
typedef struct {
    uint8_t frame[WP_MAX_FRAME_LEN];
    uint16_t len;       /* bytes currently buffered */
    uint16_t emit;      /* completed frame length, 0 = none */
} wp_rx_t;

void wp_rx_reset(wp_rx_t *rx);
uint8_t wp_rx_feed(wp_rx_t *rx, uint8_t byte); /* 1 = frame ready, else 0 */

#endif
