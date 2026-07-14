#include "spi_bus.h"

#include <stdbool.h>
#include <xc.h>

#include "mcc_generated_files/spi_host/spi1.h"
#include "mcc_generated_files/system/pins.h"

#include "semphr.h"

/* At 100 MIPS this is far longer than one byte at either configured bus
 * rate, while still bounding a peripheral/PPS fault to well below one tick. */
#define SPI_BUS_WAIT_LIMIT 60000UL
#define SPI_BUS_DRAIN_LIMIT 16u

static SemaphoreHandle_t bus_mutex;
static spi_bus_device_t current_device;
static bool bus_owned;
static bool device_selected;

static void spi_bus_deselect_all(void)
{
    EM_CS_SetHigh();
    FPGA_CS_SetHigh();
    device_selected = false;
}

static void spi_bus_stop(void)
{
    spi_bus_deselect_all();
    SPI1_Close();
    SPI1STATLbits.SPIROV = 0;
}

static spi_bus_result_t spi_bus_fail(spi_bus_result_t result)
{
    spi_bus_stop();
    return result;
}

static spi_bus_result_t spi_bus_drain_rx(void)
{
    uint8_t count;

    count = 0u;
    while (SPI1STATLbits.SPIRBE == 0U)
    {
        (void)SPI1BUFL;
        count++;
        if (count >= SPI_BUS_DRAIN_LIMIT)
        {
            return SPI_BUS_RX_TIMEOUT;
        }
    }

    return SPI_BUS_OK;
}

static spi_bus_result_t spi_bus_transfer_byte(uint8_t byte,
        uint8_t *received)
{
    uint32_t guard;
    uint8_t value;

    guard = 0UL;
    while (SPI1STATLbits.SPITBF == 1U)
    {
        guard++;
        if (guard >= SPI_BUS_WAIT_LIMIT)
        {
            return spi_bus_fail(SPI_BUS_TX_TIMEOUT);
        }
    }

    SPI1BUFL = byte;

    guard = 0UL;
    while (SPI1STATLbits.SPIRBE == 1U)
    {
        guard++;
        if (guard >= SPI_BUS_WAIT_LIMIT)
        {
            return spi_bus_fail(SPI_BUS_RX_TIMEOUT);
        }
    }

    value = (uint8_t)SPI1BUFL;
    if (received != NULL)
    {
        *received = value;
    }

    if (SPI1STATLbits.SPIROV != 0U)
    {
        SPI1STATLbits.SPIROV = 0;
        return spi_bus_fail(SPI_BUS_RX_TIMEOUT);
    }

    return SPI_BUS_OK;
}

void spi_bus_init(void)
{
    bus_owned = false;
    device_selected = false;
    spi_bus_stop();

    bus_mutex = xSemaphoreCreateMutex();
    configASSERT(bus_mutex != NULL);
}

spi_bus_result_t spi_bus_claim(spi_bus_device_t device, TickType_t timeout)
{
    uint8_t configuration;
    spi_bus_result_t result;

    if ((device != SPI_BUS_EMETER) && (device != SPI_BUS_FPGA))
    {
        return SPI_BUS_INVALID_ARGUMENT;
    }

    if (xSemaphoreTake(bus_mutex, timeout) != pdTRUE)
    {
        return SPI_BUS_BUSY;
    }

    bus_owned = true;
    current_device = device;
    spi_bus_stop();

    /* Bring-up: the iCE5 Slave-SPI interface requires the same idle-high,
     * falling-edge transmit timing as the proven emeter profile. Keep the
     * device distinction for arbitration, but use SPI_EM_CFG for both until
     * the dedicated FPGA profile is corrected in MCC. */
    configuration = SPI_EM_CFG;
    if (!SPI1_Open(configuration))
    {
        bus_owned = false;
        (void)xSemaphoreGive(bus_mutex);
        return SPI_BUS_OPEN_FAILED;
    }

    SPI1STATLbits.SPIROV = 0;
    result = spi_bus_drain_rx();
    if (result != SPI_BUS_OK)
    {
        spi_bus_stop();
        bus_owned = false;
        (void)xSemaphoreGive(bus_mutex);
        return result;
    }

    return SPI_BUS_OK;
}

spi_bus_result_t spi_bus_select(void)
{
    if (!bus_owned)
    {
        return SPI_BUS_NOT_OWNED;
    }
    if (device_selected)
    {
        return SPI_BUS_INVALID_ARGUMENT;
    }

    spi_bus_deselect_all();
    if (current_device == SPI_BUS_EMETER)
    {
        EM_CS_SetLow();
    }
    else
    {
        FPGA_CS_SetLow();
    }
    device_selected = true;
    return SPI_BUS_OK;
}

void spi_bus_deselect(void)
{
    if (bus_owned)
    {
        spi_bus_deselect_all();
    }
}

spi_bus_result_t spi_bus_exchange_byte(uint8_t byte, uint8_t *received)
{
    if (!bus_owned)
    {
        return SPI_BUS_NOT_OWNED;
    }
    if (!device_selected)
    {
        return SPI_BUS_NOT_SELECTED;
    }

    return spi_bus_transfer_byte(byte, received);
}

spi_bus_result_t spi_bus_exchange(void *buffer, size_t size)
{
    uint8_t *data;
    size_t i;
    spi_bus_result_t result;

    if ((buffer == NULL) && (size != 0u))
    {
        return SPI_BUS_INVALID_ARGUMENT;
    }

    data = (uint8_t *)buffer;
    for (i = 0u; i < size; i++)
    {
        result = spi_bus_exchange_byte(data[i], &data[i]);
        if (result != SPI_BUS_OK)
        {
            return result;
        }
    }

    return SPI_BUS_OK;
}

spi_bus_result_t spi_bus_write(const void *buffer, size_t size)
{
    const uint8_t *data;
    size_t i;
    spi_bus_result_t result;

    if ((buffer == NULL) && (size != 0u))
    {
        return SPI_BUS_INVALID_ARGUMENT;
    }

    data = (const uint8_t *)buffer;
    for (i = 0u; i < size; i++)
    {
        result = spi_bus_exchange_byte(data[i], NULL);
        if (result != SPI_BUS_OK)
        {
            return result;
        }
    }

    return SPI_BUS_OK;
}

spi_bus_result_t spi_bus_read(void *buffer, size_t size)
{
    uint8_t *data;
    size_t i;
    spi_bus_result_t result;

    if ((buffer == NULL) && (size != 0u))
    {
        return SPI_BUS_INVALID_ARGUMENT;
    }

    data = (uint8_t *)buffer;
    for (i = 0u; i < size; i++)
    {
        result = spi_bus_exchange_byte(0xFFu, &data[i]);
        if (result != SPI_BUS_OK)
        {
            return result;
        }
    }

    return SPI_BUS_OK;
}

void spi_bus_release(void)
{
    if (!bus_owned)
    {
        return;
    }

    spi_bus_stop();
    bus_owned = false;
    (void)xSemaphoreGive(bus_mutex);
}
