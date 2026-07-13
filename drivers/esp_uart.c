#include "esp_uart.h"

#include <xc.h>

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "mcc_generated_files/uart/uart2.h"

#define ESP_UART_RX_STREAM_SIZE 512u

static StreamBufferHandle_t rx_stream;

void esp_uart_init(void)
{
    /* trigger level 1: wake the reader on the first byte */
    rx_stream = xStreamBufferCreate(ESP_UART_RX_STREAM_SIZE, 1u);
    configASSERT(rx_stream != NULL);

    U2STAHbits.URXISEL = 0;     /* interrupt as soon as the FIFO holds a byte */
    IFS1bits.U2RXIF = 0;
    IPC6bits.U2RXIP = 1;        /* kernel interrupt priority: FromISR rule */
    IEC1bits.U2RXIE = 1;
}

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    BaseType_t woken = pdFALSE;
    u8 byte;

    IFS1bits.U2RXIF = 0;
    while (U2STAHbits.URXBE == 0) { /* drain the hardware FIFO */
        byte = (u8)U2RXREG;
        (void)xStreamBufferSendFromISR(rx_stream, &byte, 1u, &woken);
    }

    if (woken != pdFALSE) {
        portYIELD();
    }
}

size_t esp_uart_receive(u8 *buf, size_t max, TickType_t ticks)
{
    return xStreamBufferReceive(rx_stream, buf, max, ticks);
}

void esp_uart_send(const u8 *buf, u16 len)
{
    u16 i;

    for (i = 0; i < len; i++) {
        while (!UART2_IsTxReady()) {
        }
        UART2_Write(buf[i]);
    }
}
