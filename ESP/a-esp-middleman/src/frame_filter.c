#include "frame_filter.h"

/* Frame layout: [0]=SOF1 [1]=SOF2 [2]=HDR [3]=TYPE [4]=LEN [5..]=VALUE CRC_L CRC_H */

static void ff_reset(frame_filter_t *ff)
{
    ff->frame_len = 0;
    ff->frame_total = 0;
}

static void ff_complete(frame_filter_t *ff)
{
    const uint8_t hdr = ff->frame[2];
    const uint16_t crc_calc = wp_crc16(&ff->frame[2], (uint16_t)(3u + ff->frame[4]));
    const uint16_t crc_rx = (uint16_t)(ff->frame[ff->frame_total - 2u] |
                                       ((uint16_t)ff->frame[ff->frame_total - 1u] << 8));

    if (crc_calc != crc_rx) {
        /* Forward raw and resume after these bytes (no rescan: the client
           applies the same resync rules, rescanning would duplicate output). */
        ff->forward(ff->frame, ff->frame_total);
    } else if ((hdr & 0x0Fu) == WP_SRC_PICESP) {
        if ((hdr >> 4) == WP_VERSION) {
            ff->sound(ff->frame[3]);
        }
    } else {
        ff->forward(ff->frame, ff->frame_total);
    }

    ff_reset(ff);
}

void ff_init(frame_filter_t *ff, ff_forward_fn forward, ff_sound_fn sound)
{
    ff->forward = forward;
    ff->sound = sound;
    ff_reset(ff);
}

void ff_feed(frame_filter_t *ff, const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        const uint8_t b = data[i];

        if (ff->frame_len == 0) {
            if (b == WP_SOF1) {
                ff->frame[ff->frame_len++] = b;
            } else {
                ff->forward(&b, 1);
            }
            continue;
        }

        if (ff->frame_len == 1) {
            if (b == WP_SOF2) {
                ff->frame[ff->frame_len++] = b;
                continue;
            }
            ff->forward(ff->frame, 1); /* lone SOF1 was not a frame start */
            ff->frame_len = 0;
            if (b == WP_SOF1) {
                ff->frame[ff->frame_len++] = b;
            } else {
                ff->forward(&b, 1);
            }
            continue;
        }

        ff->frame[ff->frame_len++] = b;

        if (ff->frame_len == 5) {
            ff->frame_total = (uint16_t)(WP_OVERHEAD_LEN + ff->frame[4]);
        }
        if (ff->frame_total != 0 && ff->frame_len == ff->frame_total) {
            ff_complete(ff);
        }
    }
}

void ff_timeout(frame_filter_t *ff)
{
    if (ff->frame_len > 0) {
        ff->forward(ff->frame, ff->frame_len);
        ff_reset(ff);
    }
}
