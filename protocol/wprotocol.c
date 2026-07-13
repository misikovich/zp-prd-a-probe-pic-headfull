/* Shared firmware-side helpers for wireless protocol v1.
   Compiled by both the XC16 (PIC) and ESP-IDF (ESP) builds. */

#include <string.h>

#include "wprotocol.h"

uint16_t wp_crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = WP_CRC_INIT;
    uint16_t i;
    uint8_t bit;

    for (i = 0; i < len; i++) {
        crc ^= (uint16_t)((uint16_t)data[i] << 8);
        for (bit = 0; bit < 8; bit++) {
            if (crc & 0x8000u) {
                crc = (uint16_t)((crc << 1) ^ WP_CRC_POLY);
            } else {
                crc = (uint16_t)(crc << 1);
            }
        }
    }

    return crc;
}

void wp_rx_reset(wp_rx_t *rx)
{
    rx->len = 0;
    rx->emit = 0;
}

/* Drop the first n buffered bytes. */
static void wp_rx_drop(wp_rx_t *rx, uint16_t n)
{
    memmove(rx->frame, &rx->frame[n], (size_t)(rx->len - n));
    rx->len = (uint16_t)(rx->len - n);
}

uint8_t wp_rx_feed(wp_rx_t *rx, uint8_t byte)
{
    uint16_t total;
    uint16_t crc_rx;

    if (rx->emit != 0) { /* consume the previously returned frame */
        wp_rx_drop(rx, rx->emit);
        rx->emit = 0;
    }

    rx->frame[rx->len++] = byte;

    for (;;) {
        if (rx->len >= 1 && rx->frame[0] != WP_SOF1) {
            wp_rx_drop(rx, 1);
            continue;
        }
        if (rx->len >= 2 && rx->frame[1] != WP_SOF2) {
            wp_rx_drop(rx, 1); /* lone SOF1: drop it, rescan */
            continue;
        }
        if (rx->len < 5) {
            return 0; /* header incomplete */
        }

        total = (uint16_t)(WP_OVERHEAD_LEN + rx->frame[4]);
        if (rx->len < total) {
            return 0;
        }

        crc_rx = (uint16_t)(rx->frame[total - 2u] |
                            ((uint16_t)rx->frame[total - 1u] << 8));
        if (wp_crc16(&rx->frame[2], (uint16_t)(3u + rx->frame[4])) == crc_rx) {
            rx->emit = total;
            return 1;
        }

        /* CRC mismatch: resume scanning one byte after the SOF1 that
           started this frame (a real frame may hide inside). */
        wp_rx_drop(rx, 1);
    }
}

uint16_t wp_frame_build(uint8_t *out, uint8_t src, uint8_t type,
                        const uint8_t *value, uint8_t len)
{
    uint16_t crc;

    out[0] = WP_SOF1;
    out[1] = WP_SOF2;
    out[2] = (uint8_t)((WP_VERSION << 4) | (src & 0x0Fu));
    out[3] = type;
    out[4] = len;
    if (len > 0) {
        memcpy(&out[5], value, len);
    }

    crc = wp_crc16(&out[2], (uint16_t)(3u + len));
    out[5 + len] = (uint8_t)(crc & 0xFFu);
    out[6 + len] = (uint8_t)(crc >> 8);

    return (uint16_t)(WP_OVERHEAD_LEN + len);
}
