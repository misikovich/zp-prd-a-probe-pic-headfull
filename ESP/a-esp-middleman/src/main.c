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

static bool init_stage_ok(esp_err_t err, const char *stage)
{
    if (err == ESP_OK) {
        return true;
    }

    ESP_LOGE(TAG, "%s failed: %s", stage, esp_err_to_name(err));
    sound_request(INT_ACT_SOUND_ERR);
    return false;
}

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
    ESP_ERROR_CHECK(sound_init());
    sound_request(INT_ACT_SOUND_GENERIC); /* buzzer/task ready */

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        err = nvs_flash_erase();
        if (err == ESP_OK) {
            err = nvs_flash_init();
        }
    }
    if (!init_stage_ok(err, "NVS")) {
        return;
    }

    if (!init_stage_ok(bridge_init(), "UART bridge")) {
        return;
    }
    if (!init_stage_ok(bt_spp_init(bridge_uart_send, on_bt_conn), "Bluetooth")) {
        return;
    }

    ESP_LOGI(TAG, "Middleman up");
}
