/* Bluetooth Classic SPP acceptor: the probe side of the BT serial link.
   The desktop OS pairs with the device and exposes it as a serial port,
   which the client page opens via Web Serial. Modeled on the ESP-IDF
   bt_spp_acceptor example. */

#include "bt_spp.h"

#include <stdbool.h>

#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_check.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"
#include "esp_spp_api.h"

#define BT_DEVICE_NAME "ZP-A-PROBE"
#define SPP_SERVER_NAME "ZP_A_PROBE_SPP"

static const char *TAG = "bt_spp";

static bt_spp_rx_cb_t spp_rx_cb;
static bt_spp_conn_cb_t spp_conn_cb;
static volatile uint32_t spp_conn_handle;   /* 0 = no client connected */
static volatile bool spp_congested;

static void spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    case ESP_SPP_INIT_EVT:
        if (param->init.status == ESP_SPP_SUCCESS) {
            esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, 0, SPP_SERVER_NAME);
        } else {
            ESP_LOGE(TAG, "SPP init failed: %d", param->init.status);
        }
        break;
    case ESP_SPP_START_EVT:
        if (param->start.status == ESP_SPP_SUCCESS) {
            esp_bt_gap_set_device_name(BT_DEVICE_NAME);
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            ESP_LOGI(TAG, "SPP server started as \"%s\"", BT_DEVICE_NAME);
        } else {
            ESP_LOGE(TAG, "SPP server start failed: %d", param->start.status);
        }
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        if (param->srv_open.status == ESP_SPP_SUCCESS) {
            spp_conn_handle = param->srv_open.handle;
            spp_congested = false;
            ESP_LOGI(TAG, "Client connected, handle %" PRIu32, param->srv_open.handle);
            if (spp_conn_cb != NULL) {
                spp_conn_cb(true);
            }
        }
        break;
    case ESP_SPP_CLOSE_EVT:
        if (param->close.handle == spp_conn_handle) {
            spp_conn_handle = 0;
            spp_congested = false;
            ESP_LOGI(TAG, "Client disconnected");
            if (spp_conn_cb != NULL) {
                spp_conn_cb(false);
            }
        }
        break;
    case ESP_SPP_DATA_IND_EVT:
        if (spp_rx_cb != NULL && param->data_ind.len > 0) {
            spp_rx_cb(param->data_ind.data, param->data_ind.len);
        }
        break;
    case ESP_SPP_CONG_EVT:
        spp_congested = param->cong.cong;
        break;
    case ESP_SPP_WRITE_EVT:
        spp_congested = param->write.cong;
        break;
    default:
        break;
    }
}

static void gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)
{
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(TAG, "Paired with %s", param->auth_cmpl.device_name);
        } else {
            ESP_LOGE(TAG, "Pairing failed: %d", param->auth_cmpl.stat);
        }
        break;
    case ESP_BT_GAP_CFM_REQ_EVT:
        /* SSP just-works: accept without user interaction */
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_PIN_REQ_EVT:
        /* Legacy pairing fallback, fixed PIN 1234 */
        if (!param->pin_req.min_16_digit) {
            esp_bt_pin_code_t pin_code = {'1', '2', '3', '4'};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    default:
        break;
    }
}

void bt_spp_send(const uint8_t *data, size_t len)
{
    uint32_t handle = spp_conn_handle;

    if (handle == 0 || spp_congested || len == 0) {
        return;
    }

    esp_err_t err = esp_spp_write(handle, len, (uint8_t *)data);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "SPP write failed: %s", esp_err_to_name(err));
    }
}

esp_err_t bt_spp_init(bt_spp_rx_cb_t rx_cb, bt_spp_conn_cb_t conn_cb)
{
    spp_rx_cb = rx_cb;
    spp_conn_cb = conn_cb;

    ESP_RETURN_ON_ERROR(esp_bt_controller_mem_release(ESP_BT_MODE_BLE), TAG, "BLE mem release failed");

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_bt_controller_init(&bt_cfg), TAG, "Controller init failed");
    ESP_RETURN_ON_ERROR(esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT), TAG, "Controller enable failed");

    esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_bluedroid_init_with_cfg(&bluedroid_cfg), TAG, "Bluedroid init failed");
    ESP_RETURN_ON_ERROR(esp_bluedroid_enable(), TAG, "Bluedroid enable failed");

    ESP_RETURN_ON_ERROR(esp_bt_gap_register_callback(gap_cb), TAG, "GAP callback register failed");
    ESP_RETURN_ON_ERROR(esp_spp_register_callback(spp_cb), TAG, "SPP callback register failed");

    const esp_spp_cfg_t spp_cfg = {
        .mode = ESP_SPP_MODE_CB,
        .enable_l2cap_ertm = true,
        .tx_buffer_size = 0, /* only used in VFS mode */
    };
    ESP_RETURN_ON_ERROR(esp_spp_enhanced_init(&spp_cfg), TAG, "SPP init failed");

    /* SSP just-works: no display, no keyboard */
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_NONE;
    esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE, &iocap, sizeof(iocap));

    return ESP_OK;
}
