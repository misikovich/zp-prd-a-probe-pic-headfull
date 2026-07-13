#ifndef BT_SPP_H
#define BT_SPP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

/* Bytes received from the BT client (client -> PIC direction). Invoked from
   the Bluetooth stack task; keep the handler short (e.g. UART ring write). */
typedef void (*bt_spp_rx_cb_t)(const uint8_t *data, uint16_t len);

/* BT client session opened (true) or closed (false). Invoked from the
   Bluetooth stack task; same brevity rules as the rx callback. */
typedef void (*bt_spp_conn_cb_t)(bool connected);

/* Bring up Classic BT + Bluedroid and start the SPP server.
   NVS must be initialized first. Either callback may be NULL. */
esp_err_t bt_spp_init(bt_spp_rx_cb_t rx_cb, bt_spp_conn_cb_t conn_cb);

/* Send bytes to the connected BT client (PIC -> client direction).
   Drops the whole chunk when no client is connected or the link is
   congested (the wireless link is fire-and-forget). */
void bt_spp_send(const uint8_t *data, size_t len);

#endif
