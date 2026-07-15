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

#define SERVO_ACTIVE_SAMPLE_MS 100u
#define SERVO_IDLE_SAMPLE_MS   200u
#define SERVO_MIDPOINT_WAIT_MS 500u
#define SERVO_BASELINE_FIRST_MS 600u
#define SERVO_ENDPOINT_STEP_MS 30u
#define SERVO_ENDPOINT_TIMEOUT_MS 7000u
#define SERVO_RETURN_TIMEOUT_MS 2000u

#define SERVO_ENDPOINT_CONSECUTIVE 3u
#define SERVO_ADC_FAILURE_LIMIT    3u
#define SERVO_SENSE_MIN_RISE       64u
#define SERVO_POSITION_TOLERANCE   64u

/* SCCP6 runs at 31,250 counts per 20 ms. Servo percent selects a pulse
   between the conventional 1.0 ms and 2.0 ms endpoints. */
#define SERVO_PWM_MIN_COUNTS 1563u
#define SERVO_PWM_MAX_COUNTS 3125u
#define SERVO_PWM_MID_PERCENT 50u

#define SERVO_PHASE_SETTLE   0u
#define SERVO_PHASE_BASELINE 1u
#define SERVO_PHASE_MIN      2u
#define SERVO_PHASE_MAX      3u
#define SERVO_PHASE_RETURN   4u

typedef struct {
    u16 position[WP_BUFFER_LOG_SIZE];
    u16 sense[WP_BUFFER_LOG_SIZE];
    u16 generation;
} ServoPublished;

typedef enum {
    SERVO_READ_OK,
    SERVO_READ_RETRY,
    SERVO_READ_FAILED
} ServoReadResult;

static ServoPublished published;
static u16 staging_position[WP_BUFFER_LOG_SIZE];
static u16 staging_sense[WP_BUFFER_LOG_SIZE];
static u8 staging_count;

static TaskHandle_t worker_task;
static bool test_active;
static u8 test_status;
static TickType_t accepted_at;

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

static void servo_state_get(bool *active, u8 *status)
{
    taskENTER_CRITICAL();
    *active = test_active;
    *status = test_status;
    taskEXIT_CRITICAL();
}

static void servo_status_set(u8 status)
{
    taskENTER_CRITICAL();
    test_status = status;
    taskEXIT_CRITICAL();
}

static void servo_finish(u8 status, u8 fallback_percent)
{
    servo_pwm_set_percent(fallback_percent);
    taskENTER_CRITICAL();
    test_status = status;
    test_active = false;
    taskEXIT_CRITICAL();
}

static void servo_staging_reset(void)
{
    staging_count = 0u;
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

static ServoReadResult servo_pair_read(u16 *position, u16 *sense,
        u8 *failure_count)
{
    if (sensor_service_read_servo(position, sense)) {
        *failure_count = 0u;
        return SERVO_READ_OK;
    }

    (*failure_count)++;
    return (*failure_count >= SERVO_ADC_FAILURE_LIMIT)
            ? SERVO_READ_FAILED : SERVO_READ_RETRY;
}

static void handle_servo_test(u8 type, const u8 *value, u8 len)
{
    bool accepted;

    unused(type);
    if ((len != 1u) || (value[0] != 1u)) {
        return; /* one-shot: zero cannot cancel a running test */
    }

    taskENTER_CRITICAL();
    accepted = !test_active;
    if (accepted) {
        test_active = true;
        test_status = WP_SRV_STATUS_SEEK_MIN;
        accepted_at = xTaskGetTickCount();
    }
    taskEXIT_CRITICAL();

    if (accepted) {
        dlog("servo: command accepted");
        xTaskNotifyGive(worker_task);
    } else {
        dlog("servo: command ignored - active");
    }
}

static u8 collect_servo_act(u8 *value)
{
    bool active;
    u8 status;

    servo_state_get(&active, &status);
    unused(status);
    value[0] = active ? 1u : 0u;
    return 1u;
}

static u8 collect_servo_status(u8 *value)
{
    bool active;
    u8 status;

    servo_state_get(&active, &status);
    unused(active);
    value[0] = status;
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

static bool servo_sense_high(u16 sense, u32 threshold)
{
    return (u32)sense > threshold;
}

static bool servo_position_near(u16 position, u16 target)
{
    const u16 difference = (position >= target)
            ? (u16)(position - target) : (u16)(target - position);

    return difference <= SERVO_POSITION_TOLERANCE;
}

static TickType_t servo_accepted_time(void)
{
    TickType_t started;

    taskENTER_CRITICAL();
    started = accepted_at;
    taskEXIT_CRITICAL();
    return started;
}

static void servo_test_run(void)
{
    const TickType_t started = servo_accepted_time();
    TickType_t telemetry_last = started;
    TickType_t step_last = started;
    TickType_t return_started = started;
    TickType_t now;
    u32 elapsed_ms;
    u32 baseline_sum = 0u;
    u32 sense_threshold = 0u;
    u16 position = 0u;
    u16 sense = 0u;
    u16 min_position = 0u;
    u16 max_position = 0u;
    u8 min_percent = 0u;
    u8 max_percent = 0u;
    u8 pwm_percent = SERVO_PWM_MID_PERCENT;
    u8 phase = SERVO_PHASE_SETTLE;
    u8 baseline_count = 0u;
    u8 endpoint_high_count = 0u;
    u8 return_good_count = 0u;
    u8 adc_failure_count = 0u;
    bool telemetry_due;
    bool step_due;
    bool need_read;
    ServoReadResult read_result;

    servo_staging_reset();
    servo_pwm_set_percent(SERVO_PWM_MID_PERCENT);
    SRV_ON_SetHigh();
    dlog("servo: test start");

    forever
    {
        now = xTaskGetTickCount();
        elapsed_ms = (u32)pdTICKS_TO_MS(now - started);

        if ((phase != SERVO_PHASE_RETURN) &&
                (elapsed_ms >= SERVO_ENDPOINT_TIMEOUT_MS)) {
            dlog("servo: endpoint timeout");
            servo_finish(WP_SRV_STATUS_ENDPOINT_TIMEOUT, 0u);
            return;
        }

        if ((phase == SERVO_PHASE_SETTLE) &&
                (elapsed_ms >= SERVO_MIDPOINT_WAIT_MS)) {
            phase = SERVO_PHASE_BASELINE;
        }

        telemetry_due = (TickType_t)(now - telemetry_last) >=
                pdMS_TO_TICKS(SERVO_ACTIVE_SAMPLE_MS);
        step_due = ((phase == SERVO_PHASE_MIN) ||
                    (phase == SERVO_PHASE_MAX)) &&
                ((TickType_t)(now - step_last) >=
                    pdMS_TO_TICKS(SERVO_ENDPOINT_STEP_MS));
        need_read = telemetry_due || step_due;

        if (need_read) {
            if (telemetry_due) {
                telemetry_last = now;
            }
            if (step_due) {
                step_last = now;
            }

            read_result = servo_pair_read(&position, &sense,
                    &adc_failure_count);
            if (read_result == SERVO_READ_FAILED) {
                dlog("servo: ADC failed");
                servo_finish(WP_SRV_STATUS_ADC_FAILED, 0u);
                return;
            }
            if (read_result == SERVO_READ_OK) {
                if (telemetry_due) {
                    servo_sample_publish(position, sense);
                }

                if ((phase == SERVO_PHASE_BASELINE) && telemetry_due &&
                        (elapsed_ms >= SERVO_BASELINE_FIRST_MS)) {
                    baseline_sum += sense;
                    baseline_count++;
                    if (baseline_count >= WP_BUFFER_LOG_SIZE) {
                        const u32 baseline = baseline_sum /
                                WP_BUFFER_LOG_SIZE;
                        u32 rise = baseline / 4u;

                        if (rise < SERVO_SENSE_MIN_RISE) {
                            rise = SERVO_SENSE_MIN_RISE;
                        }
                        sense_threshold = baseline + rise;
                        phase = SERVO_PHASE_MIN;
                        pwm_percent = SERVO_PWM_MID_PERCENT - 1u;
                        servo_pwm_set_percent(pwm_percent);
                        step_last = now;
                    }
                } else if (((phase == SERVO_PHASE_MIN) ||
                            (phase == SERVO_PHASE_MAX)) && step_due) {
                    if (servo_sense_high(sense, sense_threshold)) {
                        endpoint_high_count++;
                    } else {
                        endpoint_high_count = 0u;
                    }

                    if (endpoint_high_count >= SERVO_ENDPOINT_CONSECUTIVE) {
                        endpoint_high_count = 0u;
                        if (phase == SERVO_PHASE_MIN) {
                            min_percent = pwm_percent;
                            min_position = position;
                            phase = SERVO_PHASE_MAX;
                            servo_status_set(WP_SRV_STATUS_SEEK_MAX);
                            if (pwm_percent < 100u) {
                                pwm_percent++;
                            }
                            servo_pwm_set_percent(pwm_percent);
                            step_last = now;
                        } else {
                            max_percent = pwm_percent;
                            max_position = position;
                            unused(max_percent);
                            unused(max_position);
                            phase = SERVO_PHASE_RETURN;
                            servo_status_set(WP_SRV_STATUS_RETURN_MIN);
                            servo_pwm_set_percent(min_percent);
                            return_started = now;
                            return_good_count = 0u;
                        }
                    } else if (phase == SERVO_PHASE_MIN) {
                        if (pwm_percent > 0u) {
                            pwm_percent--;
                            servo_pwm_set_percent(pwm_percent);
                        }
                    } else if (pwm_percent < 100u) {
                        pwm_percent++;
                        servo_pwm_set_percent(pwm_percent);
                    }
                } else if ((phase == SERVO_PHASE_RETURN) && telemetry_due) {
                    if (servo_position_near(position, min_position)) {
                        return_good_count++;
                    } else {
                        return_good_count = 0u;
                    }
                    if (return_good_count >= SERVO_ENDPOINT_CONSECUTIVE) {
                        dlog("servo: test pass");
                        servo_finish(WP_SRV_STATUS_PASS, min_percent);
                        return;
                    }
                }
            }
        }

        if ((phase == SERVO_PHASE_RETURN) &&
                ((TickType_t)(now - return_started) >=
                    pdMS_TO_TICKS(SERVO_RETURN_TIMEOUT_MS))) {
            dlog("servo: return failed");
            servo_finish(WP_SRV_STATUS_RETURN_FAILED, 0u);
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(SERVO_TASK_TICK_MS));
    }
}

static void servo_worker(void *parameters)
{
    TickType_t idle_last;
    u16 position;
    u16 sense;

    unused(parameters);
    idle_last = xTaskGetTickCount();
    dlog("Started Task: servo");

    forever
    {
        if (ulTaskNotifyTake(pdTRUE,
                pdMS_TO_TICKS(SERVO_TASK_TICK_MS)) > 0u) {
            servo_test_run();
            servo_staging_reset();
            idle_last = xTaskGetTickCount();
            continue;
        }

        if ((TickType_t)(xTaskGetTickCount() - idle_last) >=
                pdMS_TO_TICKS(SERVO_IDLE_SAMPLE_MS)) {
            idle_last = xTaskGetTickCount();
            if (sensor_service_read_servo(&position, &sense)) {
                servo_sample_publish(position, sense);
            }
        }
    }
}

void servo_service_init(void)
{
    BaseType_t created;

    memset(&published, 0, sizeof(published));
    servo_staging_reset();
    test_active = false;
    test_status = WP_SRV_STATUS_IDLE;
    servo_pwm_set_percent(SERVO_PWM_MID_PERCENT);
    SRV_ON_SetLow();

    created = xTaskCreate(servo_worker, "servo", SERVO_SERVICE_STACK,
            NULL, SERVO_SERVICE_PRIO, &worker_task);
    configASSERT(created == pdPASS);

    configASSERT(wproto_add_command_handler(WP_ACT_SRV_TEST,
            handle_servo_test));
    configASSERT(wproto_add_reporter(WP_ACT_SRV_TEST, SERVO_REPORT_MS,
            collect_servo_act));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_STATUS, SERVO_REPORT_MS,
            collect_servo_status));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_POS_ADC, SERVO_REPORT_MS,
            collect_servo_position));
    configASSERT(wproto_add_reporter(WP_TYPE_SRV_SENSE_ADC, SERVO_REPORT_MS,
            collect_servo_sense));
}
