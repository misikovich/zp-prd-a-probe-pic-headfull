#ifndef WPROTOCOL_H
#define WPROTOCOL_H

/*
    WIRELESS DATA TRANSFER PROTOCOL (v1)

    Product A has no integrated screen, so captured diagnosis values are
    shared over a wireless link:

        PIC <-*UART*-> ESP <-*BT*-> Client Diagnosis Device

    The ESP is a transparent bridge: it forwards bytes in both directions
    without processing, to minimize complexity and points of failure.

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
    is superseded by the next one). Frames that must not be lost
    (commands, config) are acknowledged: the receiver echoes
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

#define WP_VERSION              1u

/* HDR = (WP_VERSION << 4) | WP_SRC_x */
#define WP_SRC_PROBE            0x1u    /* PIC -> client */
#define WP_SRC_CLIENT           0x2u    /* client -> PIC */

#define WP_MAX_VALUE_LEN        255u
#define WP_OVERHEAD_LEN         7u      /* SOF(2) + HDR + TYPE + LEN + CRC(2) */
#define WP_MAX_FRAME_LEN        (WP_OVERHEAD_LEN + WP_MAX_VALUE_LEN)

#define WP_CRC_INIT             0xFFFFu
#define WP_CRC_POLY             0x1021u

#define WP_INTERBYTE_TIMEOUT_MS 50u
#define WP_ACK_TIMEOUT_MS       200u
#define WP_ACK_RETRIES          3u

typedef enum {
  WP_TYPE_INVALID      = 0x00, /* reserved, never transmitted */

  /* measurements, probe -> client, fire-and-forget */
  WP_TYPE_TEMP_AMBIENT_ADC  = 0x01, /* int16, 0-65536 */
  WP_TYPE_TEMP_SOCKET_ADC   = 0x02, /* int16, 0-65536 */
  WP_TYPE_TEMP_RCD_ADC      = 0x03, /* int16, 0-65536 */
  WP_TYPE_PWR_HWID_ADC      = 0x04, /* int16, 0-65536 */
  WP_TYPE_EM_LINK           = 0x10, /* uint8, states: 00 - all zero, FF - all F, 01 - ok */
  WP_TYPE_EM_VA             = 0x11, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_VB             = 0x12, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_VC             = 0x13, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IA             = 0x14, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IB             = 0x15, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_IC             = 0x16, /* uint16[buffer_log_size], array */
  WP_TYPE_EM_FWVER          = 0x17, /* uint8[3], 3 bytes 04FC08 */
  WP_TYPE_EM_DRDY           = 0x18, /* uint8, 0-1 bool */
  WP_TYPE_EM_FHZ            = 0x19, /* uint16, 0.01 hz/LSB */
  WP_TYPE_EM_WARN           = 0x1A, /* uint8 */
  WP_TYPE_EM_ERR            = 0x1B, /* uint8 */
  WP_TYPE_EM_TEMP           = 0x1C, /* uint16, 0-65565 */
  WP_TYPE_EM_INT            = 0x1D, /* uint8, 0-1 bool */
  WP_TYPE_EM_ALARM          = 0x1E, /* uint8, 0-1 bool */
  WP_TYPE_SRV_POS_ADC       = 0x21, /* uint16[buffer_log_size], array */
  WP_TYPE_SRV_CURR_ADC      = 0x22, /* uint16[buffer_log_size], array */
  WP_TYPE_SRV_STATE         = 0x23, /* uint8, states: FF - stop, 01 - locking, 02 - unlocking */
  WP_TYPE_GRID_NGND_ADC     = 0x31, /* uint16, 0-65565 */
  WP_TYPE_GRID_L1L2_ADC     = 0x32, /* uint16, 0-65565 */
  WP_TYPE_RCD_ERR           = 0x41, /* uint8, 0-1 bool */
  WP_TYPE_RCD_F6MA          = 0x42, /* uint8, 0-1 bool */
  WP_TYPE_RCD_F30MA         = 0x43, /* uint8, 0-1 bool */
  WP_TYPE_FPGA_CDONE        = 0x51, /* uint8, 0-1 bool */
  WP_TYPE_FPGA_INT          = 0x51, /* uint8, 0-1 bool */

  
  /* control, acknowledged */
  WP_TYPE_ACK          = 0xF0, /* uint8: TYPE being acknowledged */
  WP_TYPE_ERROR        = 0xF1, /* uint8: error code */
  WP_TYPE_HEARTBEAT    = 0xFF  /* uint8: CC */
} wp_type_t;


#endif
