#ifndef ESP_UART_H
#define ESP_UART_H

#include <stddef.h>

#include "utils.h"

/*
 * ESP wireless-link transport: UART2 @ 115200 (MCC-configured pins/baud).
 *
 * RX is interrupt-driven: _U2RXInterrupt (priority 1, the kernel interrupt
 * priority, per the rtos/ rule for FromISR use) drains the 8-byte hardware
 * FIFO into a stream buffer that esp_uart_receive blocks on. The polled MCC
 * driver alone cannot be trusted here: at 115200 more than a FIFO's worth
 * arrives per RTOS tick, and WP_TYPE_CONNECTED events are one-shot.
 *
 * TX is a polled FIFO write. Callers serialize through the wproto TX mutex.
 */

void esp_uart_init(void);
size_t esp_uart_receive(u8 *buf, size_t max, TickType_t ticks);
void esp_uart_send(const u8 *buf, u16 len);

#endif /* ESP_UART_H */
