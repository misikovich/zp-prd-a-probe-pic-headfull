/*
 * FreeRTOS application hooks.  Both hooks trap so the failure is visible
 * under a debugger; on target they halt rather than corrupt state.
 */

#include "FreeRTOS.h"
#include "task.h"

void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
    ( void ) xTask;
    ( void ) pcTaskName;

    portDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
}

void vApplicationMallocFailedHook( void )
{
    portDISABLE_INTERRUPTS();

    for( ; ; )
    {
    }
}
