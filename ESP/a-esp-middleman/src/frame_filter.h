#ifndef FRAME_FILTER_H
#define FRAME_FILTER_H

#include <stddef.h>
#include <stdint.h>

#include "wprotocol.h"

/*
    Streaming filter for the PIC -> client byte stream.

    Valid frames whose HDR source is WP_SRC_PICESP are addressed to the
    ESP: their TYPE is handed to the sound callback and the bytes are
    never forwarded. Everything else — other valid frames, and bytes
    that fail parsing (bad CRC, timeout, garbage) — goes to the forward
    callback in order, each byte exactly once, so the downstream client
    parser can resynchronize with its usual rules.

    Exception: a valid WP_SRC_PICESP frame with a mismatched protocol
    version is dropped silently (its layout cannot be trusted, and it is
    addressed to the ESP either way).

    Pure logic, no ESP-IDF dependencies: host-testable.
*/

typedef void (*ff_forward_fn)(const uint8_t *data, size_t len);
typedef void (*ff_sound_fn)(uint8_t wp_type);

typedef struct {
    ff_forward_fn forward;
    ff_sound_fn sound;
    uint8_t frame[WP_MAX_FRAME_LEN];
    uint16_t frame_len;     /* bytes collected of the current candidate frame */
    uint16_t frame_total;   /* full frame size, 0 until the LEN byte is seen */
} frame_filter_t;

void ff_init(frame_filter_t *ff, ff_forward_fn forward, ff_sound_fn sound);

/* Feed received bytes through the filter. */
void ff_feed(frame_filter_t *ff, const uint8_t *data, size_t len);

/* Interbyte timeout (WP_INTERBYTE_TIMEOUT_MS): flush any partial frame
   to the forward callback and rescan from the next byte. */
void ff_timeout(frame_filter_t *ff);

#endif
