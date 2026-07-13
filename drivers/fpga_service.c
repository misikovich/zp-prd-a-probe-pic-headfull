#include "fpga_service.h"

#include "FreeRTOS.h"
#include "task.h"

#include "fpga.h"
#include "mcc_generated_files/system/pins.h"
#include "wproto.h"

#define FPGA_SERVICE_STACK 512u /* 16-bit words; proven for RLE upload path */
#define FPGA_SERVICE_PRIO  1u
#define FPGA_REPORT_MS     100u

static TaskHandle_t worker_task;
static bool upload_active;

static bool fpga_service_active(void)
{
    bool active;

    taskENTER_CRITICAL();
    active = upload_active;
    taskEXIT_CRITICAL();
    return active;
}

static void handle_fpga_test(u8 type, const u8 *value, u8 len)
{
    bool accepted;

    unused(type);
    if ((len != 1u) || (value[0] != 1u)) {
        return; /* one-shot: zero cannot cancel an upload */
    }

    taskENTER_CRITICAL();
    accepted = !upload_active;
    if (accepted) {
        upload_active = true; /* covers both queued and loading states */
    }
    taskEXIT_CRITICAL();

    if (accepted) {
        xTaskNotifyGive(worker_task);
    }
}

static u8 collect_fpga_act(u8 *value)
{
    value[0] = fpga_service_active() ? 1u : 0u;
    return 1u;
}

static u8 collect_fpga_status(u8 *value)
{
    value[0] = fpga_service_active() ? FPGA_UPLOAD_STATUS_LOADING
                                     : fpga_upload_status();
    return 1u;
}

static u8 collect_fpga_cdone(u8 *value)
{
    value[0] = FPGA_CDONE_GetValue() ? 1u : 0u;
    return 1u;
}

static u8 collect_fpga_int(u8 *value)
{
    value[0] = FPGA_INT_GetValue() ? 1u : 0u;
    return 1u;
}

static void fpga_worker(void *parameters)
{
    unused(parameters);
    dlog("Started Task: fpga");

    forever
    {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        fpga_prog_load();

        taskENTER_CRITICAL();
        upload_active = false;
        taskEXIT_CRITICAL();
    }
}

void fpga_service_init(void)
{
    BaseType_t created;

    fpga_park();
    upload_active = false;

    created = xTaskCreate(fpga_worker, "fpga", FPGA_SERVICE_STACK, NULL,
            FPGA_SERVICE_PRIO, &worker_task);
    configASSERT(created == pdPASS);

    configASSERT(wproto_add_command_handler(WP_ACT_FPGA_TEST,
            handle_fpga_test));
    configASSERT(wproto_add_reporter(WP_ACT_FPGA_TEST, FPGA_REPORT_MS,
            collect_fpga_act));
    configASSERT(wproto_add_reporter(WP_TYPE_FPGA_STATUS, FPGA_REPORT_MS,
            collect_fpga_status));
    configASSERT(wproto_add_reporter(WP_TYPE_FPGA_CDONE, FPGA_REPORT_MS,
            collect_fpga_cdone));
    configASSERT(wproto_add_reporter(WP_TYPE_FPGA_INT, FPGA_REPORT_MS,
            collect_fpga_int));
}
