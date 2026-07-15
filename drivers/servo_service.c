#include "servo_service.h"

#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/pwm/sccp6.h"
#include "mcc_generated_files/system/pins.h"
#include "sensor_service.h"
#include "utils.h"
#include "wproto.h"

#define SERVO_SERVICE_STACK 320u /* 16-bit words */
#define SERVO_SERVICE_PRIO  1u
#define SERVO_REPORT_MS     100u
#define SERVO_TASK_TICK_MS  10u

#define SERVO_SAMPLE_MS       200u
#define SERVO_POWER_LIMIT_MS 2000u

/* SCCP6 runs at 31,250 counts per 20 ms. The mechanism is unlocked at
   100% (2 ms) and locked at 0% (1 ms). */
#define SERVO_PWM_MIN_COUNTS 1563u
#define SERVO_PWM_MAX_COUNTS 3125u
#define SERVO_PWM_MID_PERCENT 50u

typedef struct {
    u16 position[WP_BUFFER_LOG_SIZE];
    u16 sense[WP_BUFFER_LOG_SIZE];
    u16 generation;
} ServoPublished;

static ServoPublished published;
static u16 staging_position[WP_BUFFER_LOG_SIZE];
static u16 staging_sense[WP_BUFFER_LOG_SIZE];
static u8 staging_count;

static bool servo_powered;
static TickType_t power_started;

static void servo_pwm_set_percent(u8 percent)
{
    const u32 span = (u32)SERVO_PWM_MAX_COUNTS - SERVO_PWM_MIN_COUNTS;
    u16 counts;

    if (percent > 100u) {
        percent = 100u;
    }
    counts = (u16)(SERVO_PWM_MIN_COUNTS +
            (((u32)percent * span + 50u) / 100u));
    PWM_SERVO_DutyCycleSet(counts);
}

static void servo_power_on(void)
{
    taskENTER_CRITICAL();
    if (!servo_powered) {
        /* SRV_ON is active-low. Do not extend an existing power window so
           continuous commands cannot hold the mechanism against a limit. */
        power_started = xTaskGetTickCount();
        SRV_ON_SetLow();
        servo_powered = true;
    }
    taskEXIT_CRITICAL();
}

static void servo_power_off(void)
{
    taskENTER_CRITICAL();
    SRV_ON_SetHigh();
    servo_powered = false;
    taskEXIT_CRITICAL();
}

static bool servo_power_state(TickType_t *started)
{
    bool powered;

    taskENTER_CRITICAL();
    powered = servo_powered;
    if (started != NULL) {
        *started = power_started;
    }
    taskEXIT_CRITICAL();
    return powered;
}

static void servo_sample_publish(u16 position, u16 sense)
{
    const u8 index = staging_count;

    staging_position[index] = position;
    staging_sense[index] = sense;
    staging_count++;
    if (staging_count < WP_BUFFER_LOG_SIZE) {
        return;
    }

    taskENTER_CRITICAL();
    memcpy(published.position, staging_position, sizeof(published.position));
    memcpy(published.sense, staging_sense, sizeof(published.sense));
    published.generation++;
    if (published.generation == 0u) {
        published.generation = 1u;
    }
    taskEXIT_CRITICAL();
    staging_count = 0u;
}

static void handle_servo_position(u8 type, const u8 *value, u8 len)
{
    unused(type);
    if ((len != 1u) || (value[0] > 1u)) {
        return;
    }

    /* Establish the pulse before applying power. Logical 0 is the unlocked
       position and logical 1 is the locked position. */
    servo_pwm_set_percent(value[0] ? 0u : 100u);
    servo_power_on();
    dlog(value[0] ? "servo: manual locked" : "servo: manual unlocked");
}

static u8 collect_servo_power(u8 *value)
{
    value[0] = servo_power_state(NULL) ? 1u : 0u;
    return 1u;
}

static u8 collect_servo_batch(u8 *value, const u16 *samples,
        u16 *last_generation)
{
    u16 generation;
    u8 i;

    taskENTER_CRITICAL();
    generation = published.generation;
    if ((generation == 0u) || (generation == *last_generation)) {
        taskEXIT_CRITICAL();
        return 0u;
    }
    for (i = 0u; i < WP_BUFFER_LOG_SIZE; i++) {
        value[(u8)(i * 2u)] = (u8)(samples[i] & 0xFFu);
        value[(u8)(i * 2u + 1u)] = (u8)(samples[i] >> 8u);
    }
    *last_generation = generation;
    taskEXIT_CRITICAL();
    return (u8)(WP_BUFFER_LOG_SIZE * 2u);
}

static u8 collect_servo_position(u8 *value)
{
    static u16 last_generation;

    return collect_servo_batch(value, published.position, &last_generation);
}

static u8 collect_servo_sense(u8 *value)
{
    static u16 last_generation;

    return collect_servo_batch(value, published.sense, &last_generation);
}

static void servo_worker(void *parameters)
{
    TickType_t sample_last;
    TickType_t started;
    TickType_t now;
    u16 position;
    u16 sense;

    unused(parameters);
    sample_last = xTaskGetTickCount();
    dlog("Started Task: servo");

    forever
    {
        now = xTaskGetTickCount();
        if (servo_power_state(&started) &&
                ((TickType_t)(now - started) >=
                    pdMS_TO_TICKS(SERVO_POWER_LIMIT_MS))) {
            servo_power_off();
            dlog("servo: power timeout");
        }

        if ((TickType_t)(now - sample_last) >=
                pdMS_TO_TICKS(SERVO_SAMPLE_MS)) {
            sample_last = now;
            if (sensor_service_read_servo(&position, &sense)) {
                servo_sample_publish(position, sense);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(SERVO_TASK_TICK_MS));
    }
}

void servo_service_init(void)
{
    BaseType_t created;

    memset(&published, 0, sizeof(published));
    staging_count = 0u;
    servo_powered = false;
    power_started = 0u;

    /* MCC does not expose SCCP6 output polarity. The board-level servo
       signal is inverted, so flip OC6A while servo power remains off. */
    CCP6CON3Hbits.POLACE = 1u;
    servo_pwm_set_percent(SERVO_PWM_MID_PERCENT);
    SRV_ON_SetHigh();

    created = xTaskCreate(servo_worker, "servo", SERVO_SERVICE_STACK,
            NULL, SERVO_SERVICE_PRIO, NULL);
    configASSERT(created == pdPASS);

    configASSERT(wproto_add_command_handler(WP_ACT_SRV_POSITION,
            handle_servo_position));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_POWER, SERVO_REPORT_MS,
            collect_servo_power));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_POS_ADC, SERVO_REPORT_MS,
            collect_servo_position));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_SENSE_ADC, SERVO_REPORT_MS,
            collect_servo_sense));
}
