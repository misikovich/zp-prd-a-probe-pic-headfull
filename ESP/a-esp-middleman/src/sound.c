/* Buzzer sound emitter, ported from the standalone pwr-probe-esp-pio player.
   Sounds are selected by wprotocol INT_ACT_SOUND_* type and played from a
   dedicated task fed by a queue, so callers (the UART bridge) never block. */

#include "sound.h"

#include <stddef.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "wprotocol.h"

#define BUZZER_GPIO GPIO_NUM_2
#define TONE_CHANNEL LEDC_CHANNEL_0
#define TONE_DUTY_RES LEDC_TIMER_10_BIT
#define TONE_MODE LEDC_LOW_SPEED_MODE
#define TONE_TIMER LEDC_TIMER_0
#define SOUND_QUEUE_LEN 8
#define SOUND_TASK_STACK 2048
#define SOUND_TASK_PRIO 5
#define mute(ms) {0, 0, ms}
#define tones(seq) {seq, sizeof(seq) / sizeof(seq[0])}

static const char *TAG = "sound";

struct tone {
    int hz;
    int duty;
    int ms;
};

struct tone_sequence {
    const struct tone *tones;
    size_t count;
};

static const float VOLUME = 0.3f;
static const float MAX_TONE_DUTY = 0.5;

static const struct tone STARTUP_TONES[] = {
    {900, 40, 150},
    mute(20),
    {1300, 35, 150},
    mute(20),
};

static const struct tone GENERIC_TONES[] = { {1600, 80, 20}, mute(20) };
static const struct tone DOUBLE_TONES[] = { {1600, 80, 20}, mute(60), {1600, 80, 20}, mute(20) };
static const struct tone TRIPLE_TONES[] = { {1600, 80, 20}, mute(60), {1600, 80, 20}, mute(60), {1600, 80, 20}, mute(20) };
static const struct tone SWITCH_TONES[] = { {2100, 80, 20}, mute(20) };
static const struct tone OK_TONES[] = { {2700, 50, 40}, mute(20) };
static const struct tone ERR_TONES[] = { {500, 80, 100}, mute(20) };
static const struct tone CONNECT_TONES[] = { {1300, 50, 60}, mute(20), {2000, 50, 90}, mute(20) };
static const struct tone DISCONNECT_TONES[] = { {2000, 50, 60}, mute(20), {1300, 50, 90}, mute(20) };

static QueueHandle_t sound_queue;

static const struct tone_sequence *sequence_for(uint8_t wp_type)
{
    static const struct tone_sequence generic = tones(GENERIC_TONES);
    static const struct tone_sequence double_beep = tones(DOUBLE_TONES);
    static const struct tone_sequence triple = tones(TRIPLE_TONES);
    static const struct tone_sequence switch_page = tones(SWITCH_TONES);
    static const struct tone_sequence ok = tones(OK_TONES);
    static const struct tone_sequence err = tones(ERR_TONES);
    static const struct tone_sequence startup = tones(STARTUP_TONES);
    static const struct tone_sequence connect = tones(CONNECT_TONES);
    static const struct tone_sequence disconnect = tones(DISCONNECT_TONES);

    switch (wp_type) {
    case INT_ACT_SOUND_GENERIC: return &generic;
    case INT_ACT_SOUND_DOUBLE: return &double_beep;
    case INT_ACT_SOUND_TRIPLE: return &triple;
    case INT_ACT_SOUND_SWITCH: return &switch_page;
    case INT_ACT_SOUND_OK: return &ok;
    case INT_ACT_SOUND_ERR: return &err;
    case INT_ACT_SOUND_STARTUP: return &startup;
    case INT_ACT_SOUND_CONNECT: return &connect;
    case INT_ACT_SOUND_DISCONNECT: return &disconnect;
    default: return NULL;
    }
}

static void play_tone(const struct tone *tone)
{
    if (tone->hz <= 0 || tone->duty <= 0) {
        ledc_set_duty(TONE_MODE, TONE_CHANNEL, 0);
        ledc_update_duty(TONE_MODE, TONE_CHANNEL);
        vTaskDelay(pdMS_TO_TICKS(tone->ms));
        return;
    }

    const int max_duty = (1 << TONE_DUTY_RES) - 1;
    const int duty = (int)(max_duty * MAX_TONE_DUTY * (tone->duty / 100.0f) * VOLUME);

    ledc_set_freq(TONE_MODE, TONE_TIMER, tone->hz);
    ledc_set_duty(TONE_MODE, TONE_CHANNEL, duty);
    ledc_update_duty(TONE_MODE, TONE_CHANNEL);
    vTaskDelay(pdMS_TO_TICKS(tone->ms));
}

static void stop_tone(void)
{
    ledc_set_duty(TONE_MODE, TONE_CHANNEL, 0);
    ledc_update_duty(TONE_MODE, TONE_CHANNEL);
}

static void sound_task(void *arg)
{
    (void)arg;

    uint8_t wp_type = 0;

    while (true) {
        if (xQueueReceive(sound_queue, &wp_type, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        const struct tone_sequence *seq = sequence_for(wp_type);
        for (size_t i = 0; i < seq->count; i++) {
            play_tone(&seq->tones[i]);
        }
        stop_tone();
    }
}

bool sound_request(uint8_t wp_type)
{
    if (sequence_for(wp_type) == NULL) {
        ESP_LOGW(TAG, "Unknown sound type 0x%02x", wp_type);
        return false;
    }

    if (xQueueSend(sound_queue, &wp_type, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Sound queue full, dropping 0x%02x", wp_type);
        return false;
    }

    return true;
}

esp_err_t sound_init(void)
{
    const ledc_timer_config_t timer = {
        .speed_mode = TONE_MODE,
        .timer_num = TONE_TIMER,
        .duty_resolution = TONE_DUTY_RES,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "Failed to configure LEDC timer");

    const ledc_channel_config_t channel = {
        .gpio_num = BUZZER_GPIO,
        .speed_mode = TONE_MODE,
        .channel = TONE_CHANNEL,
        .timer_sel = TONE_TIMER,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {
            .output_invert = 0,
        },
        .deconfigure = false,
    };
    ESP_RETURN_ON_ERROR(ledc_channel_config(&channel), TAG, "Failed to configure LEDC channel");

    sound_queue = xQueueCreate(SOUND_QUEUE_LEN, sizeof(uint8_t));
    ESP_RETURN_ON_FALSE(sound_queue != NULL, ESP_ERR_NO_MEM, TAG, "Failed to create sound queue");

    ESP_RETURN_ON_FALSE(
        xTaskCreate(sound_task, "sound_task", SOUND_TASK_STACK, NULL, SOUND_TASK_PRIO, NULL) == pdPASS,
        ESP_ERR_NO_MEM, TAG, "Failed to create sound task");

    return ESP_OK;
}
