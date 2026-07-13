/* PIC-facing UART and the PIC -> BT direction of the bridge.
   UART pins and baud match the proven pwr-probe-esp-pio setup. */

#include "bridge.h"

#include <string.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bt_spp.h"
#include "frame_filter.h"
#include "sound.h"
#include "wprotocol.h"

#define UART_PORT UART_NUM_1
#define UART_RX_GPIO GPIO_NUM_25
#define UART_TX_GPIO GPIO_NUM_26
#define UART_BAUD_RATE 115200
#define UART_RX_BUF_SIZE 1024
#define UART_TX_BUF_SIZE 1024
#define READ_CHUNK_SIZE 256
#define READ_TIMEOUT_MS 20
#define FWD_BUF_SIZE 512
#define BRIDGE_TASK_STACK 3072
#define BRIDGE_TASK_PRIO 6

static const char *TAG = "bridge";

static frame_filter_t filter;
static uint8_t fwd_buf[FWD_BUF_SIZE];
static size_t fwd_len;

/* Coalesce filter output into larger BT writes; flushed once per read cycle
   so byte order is preserved and congestion drops stay chunk-aligned. */
static void fwd_flush(void)
{
    if (fwd_len > 0) {
        bt_spp_send(fwd_buf, fwd_len);
        fwd_len = 0;
    }
}

static void fwd_push(const uint8_t *data, size_t len)
{
    while (len > 0) {
        const size_t space = FWD_BUF_SIZE - fwd_len;
        const size_t chunk = len < space ? len : space;
        memcpy(&fwd_buf[fwd_len], data, chunk);
        fwd_len += chunk;
        data += chunk;
        len -= chunk;
        if (fwd_len == FWD_BUF_SIZE) {
            fwd_flush();
        }
    }
}

static void on_sound_frame(uint8_t wp_type)
{
    sound_request(wp_type);
}

static void bridge_task(void *arg)
{
    (void)arg;

    static uint8_t chunk[READ_CHUNK_SIZE];
    int64_t last_rx_us = 0;

    ff_init(&filter, fwd_push, on_sound_frame);

    while (true) {
        const int n = uart_read_bytes(UART_PORT, chunk, sizeof(chunk), pdMS_TO_TICKS(READ_TIMEOUT_MS));
        if (n > 0) {
            last_rx_us = esp_timer_get_time();
            ff_feed(&filter, chunk, (size_t)n);
        } else if (filter.frame_len > 0 &&
                   esp_timer_get_time() - last_rx_us > (int64_t)WP_INTERBYTE_TIMEOUT_MS * 1000) {
            ff_timeout(&filter);
        }
        fwd_flush();
    }
}

void bridge_uart_send(const uint8_t *data, uint16_t len)
{
    uart_write_bytes(UART_PORT, data, len);
}

esp_err_t bridge_init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_RETURN_ON_ERROR(uart_driver_install(UART_PORT, UART_RX_BUF_SIZE, UART_TX_BUF_SIZE, 0, NULL, 0),
        TAG, "Failed to install UART driver");
    ESP_RETURN_ON_ERROR(uart_param_config(UART_PORT, &uart_config), TAG, "Failed to configure UART");
    ESP_RETURN_ON_ERROR(uart_set_pin(UART_PORT, UART_TX_GPIO, UART_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
        TAG, "Failed to set UART pins");

    ESP_RETURN_ON_FALSE(
        xTaskCreate(bridge_task, "bridge_task", BRIDGE_TASK_STACK, NULL, BRIDGE_TASK_PRIO, NULL) == pdPASS,
        ESP_ERR_NO_MEM, TAG, "Failed to create bridge task");

    return ESP_OK;
}
