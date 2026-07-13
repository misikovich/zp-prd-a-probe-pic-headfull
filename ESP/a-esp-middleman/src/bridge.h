#ifndef BRIDGE_H
#define BRIDGE_H

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

/* PIC-facing UART plus the PIC -> BT frame filter task. */
esp_err_t bridge_init(void);

/* Client -> PIC passthrough: write bytes to the PIC UART unmodified.
   Used as the BT SPP receive callback. */
void bridge_uart_send(const uint8_t *data, uint16_t len);

#endif
