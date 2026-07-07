/*
 * FreeRTOS configuration for the Product A probe (dsPIC33CK256MP506, XC16).
 *
 * See https://www.freertos.org/a00110.html for the meaning of each option.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* xc.h provides SET_CPU_IPL() used by portmacro.h. */
#include <xc.h>

/* Instruction clock FCY = Fosc / 2.  MCC configures Fosc = 200 MHz from the
 * FRC + PLL (see config.mcc/mcc_generated_files/system/src/clock.c); keep
 * this in sync with the MCC clock setup. */
#define configCPU_CLOCK_HZ                      100000000UL

#define configUSE_PREEMPTION                    1
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_16_BITS
#define configMAX_PRIORITIES                    4
#define configMINIMAL_STACK_SIZE                200 /* 16-bit words */
#define configMAX_TASK_NAME_LEN                 8
#define configIDLE_SHOULD_YIELD                 1

/* The dsPIC port requires the kernel (and any ISR that uses a ...FromISR()
 * API) to run at interrupt priority 1. */
#define configKERNEL_INTERRUPT_PRIORITY         1

/* Memory: task stacks and kernel objects come from heap_4's static array.
 * The device has 24 KB of RAM. */
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) 8192 )

/* Synchronisation primitives. */
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configQUEUE_REGISTRY_SIZE               0

/* Software timers. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH                8
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/* Hooks and debug aids (implemented in rtos/rtos_hooks.c). */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1

#define configASSERT( x )                       \
    if( ( x ) == 0 )                            \
    {                                           \
        portDISABLE_INTERRUPTS();               \
        for( ; ; ) {}                           \
    }

/* API functions to include. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1

#endif /* FREERTOS_CONFIG_H */
