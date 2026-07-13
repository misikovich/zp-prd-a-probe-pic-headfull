#ifndef WPROTO_H
#define WPROTO_H

#include <stdbool.h>

#include "utils.h"
#include "wprotocol.h"

/*
 * Wireless protocol engine: owns the ESP UART link.
 *
 * RX: parses the inbound stream (frames from the client and from the ESP,
 * both src WP_SRC_CLIENT) and dispatches them; WP_TYPE_CONNECTED changes
 * trigger the connect/disconnect sounds on the ESP.
 *
 * TX: periodic "reporters" stream measurements to the client. Adding a
 * reading is one collect function plus one wproto_add_reporter call:
 *
 *     static u8 collect_thing(u8 *value) {
 *         value[0] = read_thing();
 *         return 1u;                    // LEN; return 0 to skip a cycle
 *     }
 *     wproto_add_reporter(WP_TYPE_THING, 500u, collect_thing);
 *
 * Collect functions run in the wproto task (not an ISR); they may take
 * mutexes and block briefly. VALUE is at most WPROTO_VALUE_MAX bytes.
 */

#define WPROTO_VALUE_MAX 32u

typedef u8 (*wproto_collect_fn)(u8 *value);

/* Init the link and start the engine task. Registers the built-in
   WP_TYPE_HEARTBEAT reporter. Call after SYSTEM_Initialize. */
void wproto_init(void);

/* Register a periodic reporter. Returns false when the table is full.
   Call before or after wproto_init, from one task (main setup). */
bool wproto_add_reporter(u8 type, u16 period_ms, wproto_collect_fn collect);

/* Queue a sound on the ESP (INT_ACT_SOUND_*). Callable from any task. */
void wproto_sound(u8 sound_type);

/* Last WP_TYPE_CONNECTED value reported by the ESP. */
bool wproto_connected(void);

#endif /* WPROTO_H */
