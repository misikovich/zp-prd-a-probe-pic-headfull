
#include "mcc_generated_files/system/system.h"
#include <xc.h>
#include "utils.h"

#include "FreeRTOS.h"
#include "task.h"

#include "spi_bus.h"
#include "emeter_service.h"
#include "fpga_service.h"
#include "rgbw.h"
#include "wproto.h"
/*
    Main application
*/

static void heartbeat_task(void *parameters)
{
    unused(parameters);
    task_hold(5u);
    dlog("Started Task: heartbeat_task");

    /* The ESP32 takes longer to boot and initialize its buzzer/bridge. */
    task_hold(1000u);
    wproto_sound(INT_ACT_SOUND_STARTUP);

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


/* Sleep in short slices; true when the connection state changes under us. */
static bool led_watch(u32 ms, bool connected_now)
{
    u32 step;

    while (ms > 0u) {
        step = (ms > 50u) ? 50u : ms;
        task_hold(step);
        ms -= step;
        if (wproto_connected() != connected_now) {
            return true;
        }
    }
    return false;
}

static void led_flash_twice(RGBW_STATE color)
{
    u8 i;

    for (i = 0u; i < 2u; i++) {
        rgbw_hold_transition(color, 100u);
        rgbw_hold_transition(RGBW_CLEAR, 180u);
    }
}

/* Run one dim/bright breathing cycle, but stop promptly when the BT state
   changes so the corresponding connection flash is not delayed. */
static bool led_breathe(RGBW_STATE dim, RGBW_STATE bright,
        bool connected_now)
{
    rgbw_new_transition(dim, 800u);
    if (led_watch(800u, connected_now)) {
        return true;
    }

    rgbw_new_transition(bright, 800u);
    return led_watch(800u, connected_now);
}

/* Status LED: white breathing while waiting for a client, green breathing
   while attached, plus two flashes when the connection state changes. */
static void led_status_task(void *parameters)
{
    /* Percentages are in the gamma-corrected 0..255 input domain. */
    static const RGBW_STATE wait_dim = { 0u, 25u, 77u, 0u };       /* 30% */
    static const RGBW_STATE wait_bright = { 0u, 50u, 179u, 0u };  /* 70% */
    static const RGBW_STATE connected_dim = { 0u, 51u, 12u, 0u };      /* GREENF 20% */
    static const RGBW_STATE connected_bright = { 0u, 128u, 30u, 0u }; /* GREENF 50% */
    bool prev_connected = false;
    bool connected;

    unused(parameters);
    task_hold(15u);
    dlog("Started Task: led_status");

    forever
    {
        connected = wproto_connected();

        if (connected != prev_connected) {
            prev_connected = connected;
            led_flash_twice(connected ? RGBW_GREENF : RGBW_ORANGEF);
            continue;
        }

        if (connected) {
            (void)led_breathe(connected_dim, connected_bright, connected);
        } else {
            (void)led_breathe(wait_dim, wait_bright, connected);
        }
    }
}

int main(void)
{
    SYSTEM_Initialize();
    dlog("");
    dlog("System Initialized");

    spi_bus_init();
    rgbw_init();
    wproto_init();
    fpga_service_init();
    emeter_service_init();
    dlog("Drivers Initialized");

    xTaskCreate(heartbeat_task, "beat", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(rgbw_ticker, "rgbw", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(led_status_task, "led", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    dlog("Tasks Created");

    /* Never returns; the port takes over Timer1 for the tick. */
    vTaskStartScheduler();

    /* Only reached if there was not enough heap for the idle task. */
    forever;
}
