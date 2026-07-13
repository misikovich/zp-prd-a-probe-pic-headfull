/* Product A probe wireless middleman: PIC <-UART-> ESP <-BT SPP-> client.
   Forwards the wprotocol byte stream in both directions, consuming frames
   addressed to the ESP (src WP_SRC_PICESP: buzzer sound commands). */

#include "esp_log.h"
#include "nvs_flash.h"

#include "bridge.h"
#include "bt_spp.h"
#include "sound.h"
#include "wprotocol.h"

static const char *TAG = "main";

/* Tell the PIC the BT client session state. Sent with src WP_SRC_CLIENT so
   the PIC parses one inbound source. Any reaction (e.g. a connect sound
   commanded back as INT_ACT_SOUND_*) is the PIC's decision. */
static void on_bt_conn(bool connected)
{
    uint8_t frame[WP_OVERHEAD_LEN + 1];
    const uint8_t value = connected ? 1 : 0;
    const uint16_t len = wp_frame_build(frame, WP_SRC_CLIENT, WP_TYPE_CONNECTED, &value, 1);

    bridge_uart_send(frame, len);
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(sound_init());
    ESP_ERROR_CHECK(bridge_init());
    ESP_ERROR_CHECK(bt_spp_init(bridge_uart_send, on_bt_conn));
    sound_request(INT_ACT_SOUND_OK);

    ESP_LOGI(TAG, "Middleman up");
}
