
#include "mcc_generated_files/system/system.h"
#include <xc.h>
#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include "spi_bus.h"
#include "emeter.h"
#include "fpga.h"
#include "rgbw.h"
/*
    Main application
*/

static void heartbeat_task(void *parameters)
{
    unused(parameters);
    task_hold(5u);
    dlog("Started Task: heartbeat_task");

    forever
    {
        task_hold(500u);
    }
}

static void rgbw_ticker(void *parameters)
{
    unused(parameters);
    task_hold(10u);
    dlog("Started Task: rgbw_ticker");

    forever
    {
        rgbw_tick();
    }
}


static void hello_oneshot(void *parameters)
{
    unused(parameters);
    task_hold(15u);
    dlog("Started Task: hello_oneshot");
    task_hold(10u);

    rgbw_hold_transition(RGBW_WHITE,    100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_RED,      100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_GREEN,    100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_BLUE,     100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    task_hold(1200u);
    rgbw_hold_transition(RGBW_WHITEF,    200u);
    task_hold(500u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_REDF,     100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_GREENF,   100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);
    rgbw_hold_transition(RGBW_BLUEF,    100u);
    rgbw_hold_transition(RGBW_CLEAR,    500u);

    vTaskDelete(NULL);
}

int main(void)
{
    SYSTEM_Initialize();
    dlog("");
    dlog("System Initialized");

    spi_bus_init();
    fpga_park();
    emeter_init();
    rgbw_init();
    dlog("Drivers Initialized");

    xTaskCreate(heartbeat_task, "beat", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(rgbw_ticker, "rgbw", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(hello_oneshot, "hello", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    dlog("Tasks Created");

    /* Never returns; the port takes over Timer1 for the tick. */
    vTaskStartScheduler();

    /* Only reached if there was not enough heap for the idle task. */
    forever;
}
