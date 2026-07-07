#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "mcc_generated_files/uart/uart1.h"
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#define unused(f) (void)(f)
#define forever for (;;)
#define task_hold(ms) vTaskDelay(pdMS_TO_TICKS((ms)))

static inline void dlog_write(const char *text)
{
    while (*text != '\0') {
        while (!UART1_IsTxReady()) {
        }
        UART1_Write((uint8_t)*text);
        text++;
    }
}

static inline void dlog(const char *line)
{
    dlog_write(line);
    dlog_write("\r\n");
}


#endif
