/*
 * Shared SPI1 bus arbiter for the emeter and FPGA.
 *
 * Both devices sit on SPI1 with their own chip selects.  A single SPI
 * configuration (SPI1_EM_CFG, the slower emeter clock) is used for both, so
 * claiming the bus never reconfigures the peripheral.  Ownership (FreeRTOS
 * mutex) is separate from chip select: the emeter frames each 5-byte
 * transaction with its CS while holding the bus for a whole poll sweep, and
 * the FPGA holds CS low for an entire bitstream upload.
 *
 * emeter.c and fpga.c must not touch SPI1_* or the CS pin macros directly.
 * Task context only — the MCC driver is blocking/polled and the mutex
 * cannot be used from an ISR.
 */

#ifndef SPI_BUS_H
#define SPI_BUS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"

typedef enum
{
    SPI_BUS_EMETER,
    SPI_BUS_FPGA,
} spi_bus_device_t;

/* Deselect both devices and create the bus mutex; call once from main()
 * before the scheduler starts. */
void spi_bus_init(void);

/* Take exclusive ownership of SPI1 (no chip select is touched).
 * Returns false if the bus could not be acquired within timeout. */
bool spi_bus_claim(TickType_t timeout);

/* Release ownership; all chip selects must already be deasserted. */
void spi_bus_release(void);

/* Chip select control; only valid while owning the bus. */
void spi_bus_select(spi_bus_device_t device);
void spi_bus_deselect(spi_bus_device_t device);

/* Transfers; only valid while owning the bus. */
uint8_t spi_bus_exchange_byte(uint8_t byte);
void spi_bus_exchange(void *buffer, size_t size); /* in-place tx/rx */
void spi_bus_write(const void *buffer, size_t size);
void spi_bus_read(void *buffer, size_t size);

#endif /* SPI_BUS_H */
