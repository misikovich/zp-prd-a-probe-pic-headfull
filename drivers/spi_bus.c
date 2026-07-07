#include "spi_bus.h"

#include "mcc_generated_files/spi_host/spi1.h"
#include "mcc_generated_files/system/pins.h"

#include "semphr.h"

/* Single shared configuration: the emeter's (slower) clock and mode are
 * used for both devices. */
#define SPI_BUS_CONFIG SPI1_EM_CFG

static SemaphoreHandle_t bus_mutex;

void spi_bus_init(void)
{
    /* MCC's initial latch values leave both selects asserted. */
    EM_CS_SetHigh();
    FPGA_CS_SetHigh();

    bus_mutex = xSemaphoreCreateMutex();
    configASSERT(bus_mutex != NULL);
}

bool spi_bus_claim(TickType_t timeout)
{
    if (xSemaphoreTake(bus_mutex, timeout) != pdTRUE)
    {
        return false;
    }

    if (!SPI1_Open(SPI_BUS_CONFIG))
    {
        /* Cannot happen while the mutex is held and everyone closes on
         * release; treat as a programming error. */
        (void)xSemaphoreGive(bus_mutex);
        return false;
    }

    return true;
}

void spi_bus_release(void)
{
    SPI1_Close();
    (void)xSemaphoreGive(bus_mutex);
}

void spi_bus_select(spi_bus_device_t device)
{
    if (device == SPI_BUS_EMETER)
    {
        EM_CS_SetLow();
    }
    else
    {
        FPGA_CS_SetLow();
    }
}

void spi_bus_deselect(spi_bus_device_t device)
{
    if (device == SPI_BUS_EMETER)
    {
        EM_CS_SetHigh();
    }
    else
    {
        FPGA_CS_SetHigh();
    }
}

uint8_t spi_bus_exchange_byte(uint8_t byte)
{
    return SPI1_ByteExchange(byte);
}

void spi_bus_exchange(void *buffer, size_t size)
{
    SPI1_BufferExchange(buffer, size);
}

void spi_bus_write(const void *buffer, size_t size)
{
    /* MCC prototype is not const-correct; BufferWrite does not modify. */
    SPI1_BufferWrite((void *)buffer, size);
}

void spi_bus_read(void *buffer, size_t size)
{
    SPI1_BufferRead(buffer, size);
}
