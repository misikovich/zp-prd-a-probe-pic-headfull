#include "rcd_service.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/system/pins.h"
#include "wproto.h"

#define RCD_SERVICE_STACK 256u /* 16-bit words; no large automatic buffers */
#define RCD_SERVICE_PRIO  1u
#define RCD_REPORT_MS     100u
#define RCD_STARTUP_MS    300u
#define RCD_MIN_ACTIVE_MS 200u
#define RCD_TEST_PULSE_MS 100u
#define RCD_POLL_MS       10u
#define RCD_F6_WAIT_MS    700u
#define RCD_F30_WAIT_MS   700u
/* Variants keep their trip indications asserted for different portions of
   the internally generated test-current sequence. Measure recovery from the
   observed 30 mA assertion with enough margin for the complete sequence. */
#define RCD_RECOVERY_MS   2500u
#define RCD_ERR_SETTLE_MS 100u

static TaskHandle_t worker_task;
static bool test_active;
static u8 test_status;
static TickType_t service_started;
static TickType_t log_test_started;
static u8 log_err_raw;
static u8 log_f6_raw;
static u8 log_f30_raw;
static bool log_pins_valid;

static void rcd_log_append_text(char **cursor, const char *text)
{
    while (*text != '\0') {
        **cursor = *text;
        (*cursor)++;
        text++;
    }
}

static void rcd_log_append_u32(char **cursor, u32 value)
{
    char reversed[10];
    u8 count = 0u;

    do {
        reversed[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while ((value != 0u) && (count < sizeof(reversed)));

    while (count > 0u) {
        **cursor = reversed[--count];
        (*cursor)++;
    }
}

static void rcd_log_snapshot(const char *event)
{
    char line[96];
    char *cursor = line;
    const u8 err_raw = RCD_ERR_GetValue() ? 1u : 0u;
    const u8 f6_raw = RCD_F6MA_DC_GetValue() ? 1u : 0u;
    const u8 f30_raw = RCD_F30MA_AC_GetValue() ? 1u : 0u;
    const u8 test_raw = RCD_TEST_GetValue() ? 1u : 0u;
    const u32 elapsed_ms = (u32)pdTICKS_TO_MS(
            xTaskGetTickCount() - log_test_started);

    rcd_log_append_text(&cursor, "rcd: ");
    rcd_log_append_text(&cursor, event);
    rcd_log_append_text(&cursor, " t=");
    rcd_log_append_u32(&cursor, elapsed_ms);
    rcd_log_append_text(&cursor, "ms test=");
    rcd_log_append_u32(&cursor, test_raw);
    rcd_log_append_text(&cursor, " err_raw=");
    rcd_log_append_u32(&cursor, err_raw);
    rcd_log_append_text(&cursor, " err=");
    rcd_log_append_u32(&cursor, err_raw);
    rcd_log_append_text(&cursor, " f6=");
    rcd_log_append_u32(&cursor, f6_raw);
    rcd_log_append_text(&cursor, " f30=");
    rcd_log_append_u32(&cursor, f30_raw);
    *cursor = '\0';
    dlog(line);

    log_err_raw = err_raw;
    log_f6_raw = f6_raw;
    log_f30_raw = f30_raw;
    log_pins_valid = true;
}

static void rcd_log_pin_changes(void)
{
    const u8 err_raw = RCD_ERR_GetValue() ? 1u : 0u;
    const u8 f6_raw = RCD_F6MA_DC_GetValue() ? 1u : 0u;
    const u8 f30_raw = RCD_F30MA_AC_GetValue() ? 1u : 0u;

    if ((!log_pins_valid) || (err_raw != log_err_raw) ||
            (f6_raw != log_f6_raw) || (f30_raw != log_f30_raw)) {
        rcd_log_snapshot("pins changed");
    }
}

static bool rcd_pin_asserted(u16 pin_value)
{
    return pin_value != 0u;
}

static bool rcd_error_asserted(void)
{
    /* RD6 is low in the healthy idle state. It rises as part of the internal
       self-test, so only the final settled level determines test success. */
    return RCD_ERR_GetValue() != 0u;
}

static bool rcd_f6_asserted(void)
{
    return rcd_pin_asserted(RCD_F6MA_DC_GetValue());
}

static bool rcd_f30_asserted(void)
{
    return rcd_pin_asserted(RCD_F30MA_AC_GetValue());
}

static void rcd_state_get(bool *active, u8 *status)
{
    taskENTER_CRITICAL();
    *active = test_active;
    *status = test_status;
    taskEXIT_CRITICAL();
}

static void rcd_test_finish(u8 status)
{
    taskENTER_CRITICAL();
    test_status = status;
    test_active = false;
    taskEXIT_CRITICAL();
}

static void handle_rcd_test(u8 type, const u8 *value, u8 len)
{
    bool accepted;

    unused(type);
    if ((len != 1u) || (value[0] != 1u)) {
        return; /* one-shot: zero cannot cancel a sensor self-test */
    }

    taskENTER_CRITICAL();
    accepted = !test_active;
    if (accepted) {
        test_active = true; /* covers queued, pulse, and validation states */
        test_status = WP_RCD_STATUS_RUNNING;
    }
    taskEXIT_CRITICAL();

    if (accepted) {
        dlog("rcd: command queued");
        xTaskNotifyGive(worker_task);
    } else {
        dlog("rcd: command ignored - active");
    }
}

static u8 collect_rcd_act(u8 *value)
{
    bool active;
    u8 status;

    rcd_state_get(&active, &status);
    unused(status);
    value[0] = active ? 1u : 0u;
    return 1u;
}

static u8 collect_rcd_status(u8 *value)
{
    bool active;
    u8 status;

    rcd_state_get(&active, &status);
    value[0] = active ? WP_RCD_STATUS_RUNNING : status;
    return 1u;
}

static u8 collect_rcd_err(u8 *value)
{
    value[0] = rcd_error_asserted() ? 1u : 0u;
    return 1u;
}

static u8 collect_rcd_f6ma(u8 *value)
{
    value[0] = rcd_f6_asserted() ? 1u : 0u;
    return 1u;
}

static u8 collect_rcd_f30ma(u8 *value)
{
    value[0] = rcd_f30_asserted() ? 1u : 0u;
    return 1u;
}

static u8 wait_for_f6(void)
{
    const TickType_t start = xTaskGetTickCount();
    bool f30_seen = false;

    rcd_log_snapshot("wait 6mA");
    forever
    {
        rcd_log_pin_changes();
        if (rcd_f30_asserted()) {
            f30_seen = true;
        }
        if (rcd_f6_asserted()) {
            rcd_log_snapshot(f30_seen ? "6mA with early 30mA"
                                          : "6mA detected");
            return f30_seen ? WP_RCD_STATUS_SEQUENCE_FAILED
                            : WP_RCD_STATUS_RUNNING;
        }
        if ((TickType_t)(xTaskGetTickCount() - start) >=
                pdMS_TO_TICKS(RCD_F6_WAIT_MS)) {
            rcd_log_snapshot("6mA timeout");
            return WP_RCD_STATUS_F6_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(RCD_POLL_MS));
    }
}

static u8 wait_for_f30(void)
{
    const TickType_t start = xTaskGetTickCount();

    rcd_log_snapshot("wait 30mA");
    forever
    {
        rcd_log_pin_changes();
        if (rcd_f30_asserted()) {
            rcd_log_snapshot("30mA detected");
            return WP_RCD_STATUS_RUNNING;
        }
        if ((TickType_t)(xTaskGetTickCount() - start) >=
                pdMS_TO_TICKS(RCD_F30_WAIT_MS)) {
            rcd_log_snapshot("30mA timeout");
            return WP_RCD_STATUS_F30_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(RCD_POLL_MS));
    }
}

static u8 wait_for_recovery(void)
{
    const TickType_t start = xTaskGetTickCount();
    TickType_t settle_start;

    rcd_log_snapshot("wait recovery");
    forever
    {
        rcd_log_pin_changes();
        if ((!rcd_f6_asserted()) && (!rcd_f30_asserted())) {
            /* ERROR may remain asserted while the function test performs
               its offset update. Judge it only after the trip outputs have
               completed their sequence and had time to settle. */
            rcd_log_snapshot("trips recovered");
            settle_start = xTaskGetTickCount();
            while ((TickType_t)(xTaskGetTickCount() - settle_start) <
                    pdMS_TO_TICKS(RCD_ERR_SETTLE_MS)) {
                rcd_log_pin_changes();
                vTaskDelay(pdMS_TO_TICKS(RCD_POLL_MS));
            }
            rcd_log_pin_changes();
            rcd_log_snapshot("post-error-settle");
            return rcd_error_asserted() ? WP_RCD_STATUS_SENSOR_ERROR
                                        : WP_RCD_STATUS_PASS;
        }
        if ((TickType_t)(xTaskGetTickCount() - start) >=
                pdMS_TO_TICKS(RCD_RECOVERY_MS)) {
            rcd_log_snapshot("recovery timeout");
            return WP_RCD_STATUS_RECOVERY_TIMEOUT;
        }
        vTaskDelay(pdMS_TO_TICKS(RCD_POLL_MS));
    }
}

static u8 rcd_test_run(void)
{
    const TickType_t startup_ticks = pdMS_TO_TICKS(RCD_STARTUP_MS);
    TickType_t startup_elapsed;
    u8 status;

    startup_elapsed = xTaskGetTickCount() - service_started;
    if (startup_elapsed < startup_ticks) {
        vTaskDelay(startup_ticks - startup_elapsed);
    }

    rcd_log_snapshot("precheck");
    if (rcd_f6_asserted() || rcd_f30_asserted()) {
        rcd_log_snapshot("initial trip asserted");
        return WP_RCD_STATUS_INITIAL_FAULT;
    }

    RCD_TEST_SetLow();
    rcd_log_snapshot("test asserted");
    vTaskDelay(pdMS_TO_TICKS(RCD_TEST_PULSE_MS));
    RCD_TEST_SetHigh();
    rcd_log_snapshot("test released");

    status = wait_for_f6();
    if (status != WP_RCD_STATUS_RUNNING) {
        return status;
    }
    status = wait_for_f30();
    if (status != WP_RCD_STATUS_RUNNING) {
        return status;
    }
    return wait_for_recovery();
}

static const char *rcd_status_log(u8 status)
{
    switch (status) {
    case WP_RCD_STATUS_PASS:
        return "rcd: test pass";
    case WP_RCD_STATUS_INITIAL_FAULT:
        return "rcd: initial trip asserted";
    case WP_RCD_STATUS_F6_TIMEOUT:
        return "rcd: 6mA timeout";
    case WP_RCD_STATUS_F30_TIMEOUT:
        return "rcd: 30mA timeout";
    case WP_RCD_STATUS_RECOVERY_TIMEOUT:
        return "rcd: recovery timeout";
    case WP_RCD_STATUS_SENSOR_ERROR:
        return "rcd: sensor error";
    case WP_RCD_STATUS_SEQUENCE_FAILED:
        return "rcd: output sequence failed";
    default:
        return "rcd: unknown result";
    }
}

static void rcd_worker(void *parameters)
{
    TickType_t active_started;
    TickType_t active_elapsed;
    u8 status;

    unused(parameters);
    dlog("Started Task: rcd");

    forever
    {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        active_started = xTaskGetTickCount();
        log_test_started = active_started;
        log_pins_valid = false;
        dlog("rcd: test start");
        status = rcd_test_run();
        RCD_TEST_SetHigh(); /* fail-safe idle level on every exit path */
        rcd_log_snapshot("final pins");
        dlog(rcd_status_log(status));

        /* Give the 100 ms ACT reporter time to expose the accepted command,
           including failures detected immediately by the initial precheck. */
        active_elapsed = xTaskGetTickCount() - active_started;
        if (active_elapsed < pdMS_TO_TICKS(RCD_MIN_ACTIVE_MS)) {
            vTaskDelay(pdMS_TO_TICKS(RCD_MIN_ACTIVE_MS) - active_elapsed);
        }
        rcd_test_finish(status);
    }
}

void rcd_service_init(void)
{
    BaseType_t created;

    RCD_TEST_SetHigh();
    test_active = false;
    test_status = WP_RCD_STATUS_IDLE;
    service_started = xTaskGetTickCount();

    created = xTaskCreate(rcd_worker, "rcd", RCD_SERVICE_STACK, NULL,
            RCD_SERVICE_PRIO, &worker_task);
    configASSERT(created == pdPASS);

    configASSERT(wproto_add_command_handler(WP_ACT_RCD_TEST,
            handle_rcd_test));
    configASSERT(wproto_add_reporter(WP_ACT_RCD_TEST, RCD_REPORT_MS,
            collect_rcd_act));
    configASSERT(wproto_add_reporter(WP_TYPE_RCD_STATUS, RCD_REPORT_MS,
            collect_rcd_status));
    configASSERT(wproto_add_reporter(WP_TYPE_RCD_ERR, RCD_REPORT_MS,
            collect_rcd_err));
    configASSERT(wproto_add_reporter(WP_TYPE_RCD_F6MA, RCD_REPORT_MS,
            collect_rcd_f6ma));
    configASSERT(wproto_add_reporter(WP_TYPE_RCD_F30MA, RCD_REPORT_MS,
            collect_rcd_f30ma));
}
