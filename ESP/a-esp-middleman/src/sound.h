#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

/* LEDC buzzer driven by a dedicated task; playback never blocks the caller. */
esp_err_t sound_init(void);

/* Queue an INT_ACT_SOUND_* type for playback. Non-blocking; returns false
   (and logs) for unknown types or a full queue. Safe to call from any task. */
bool sound_request(uint8_t wp_type);

#endif
