/*
 * Shared SPI1 bus service for the emeter and FPGA.
 *
 * This module exclusively owns the SPI1 peripheral, both chip selects, the
 * per-device MCC configurations, and the FreeRTOS mutex. Device drivers must
 * not call SPI1_* or drive EM_CS/FPGA_CS directly.
 *
 * Task context only: the mutex cannot be used from an ISR.
 */

#ifndef SPI_BUS_H
#define SPI_BUS_H

#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"

typedef enum
{
    SPI_BUS_EMETER,
    SPI_BUS_FPGA,
} spi_bus_device_t;

typedef enum
{
    SPI_BUS_OK = 0,
    SPI_BUS_BUSY,
    SPI_BUS_INVALID_ARGUMENT,
    SPI_BUS_OPEN_FAILED,
    SPI_BUS_NOT_OWNED,
    SPI_BUS_NOT_SELECTED,
    SPI_BUS_TX_TIMEOUT,
    SPI_BUS_RX_TIMEOUT,
} spi_bus_result_t;

/* Deselect both devices and create the bus mutex; call once from main()
 * before the scheduler starts. */
void spi_bus_init(void);

/* Claim exclusive ownership and open SPI1 with the requested device's
 * configuration. Chip select remains deasserted until spi_bus_select(). */
spi_bus_result_t spi_bus_claim(spi_bus_device_t device, TickType_t timeout);

/* Select/deselect the device named by the successful claim. */
spi_bus_result_t spi_bus_select(void);
void spi_bus_deselect(void);

/* Bounded transfers. On failure SPI1 is stopped and both devices are
 * deselected, but the caller still owns the mutex and must release it. */
spi_bus_result_t spi_bus_exchange_byte(uint8_t byte, uint8_t *received);
spi_bus_result_t spi_bus_exchange(void *buffer, size_t size);
spi_bus_result_t spi_bus_write(const void *buffer, size_t size);
spi_bus_result_t spi_bus_read(void *buffer, size_t size);

/* Safely deselect both devices, stop SPI1, and release ownership. */
void spi_bus_release(void);

#endif /* SPI_BUS_H */
