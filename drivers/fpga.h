#ifndef FPGA_H
#define FPGA_H

#include <stdbool.h>

#include "utils.h"
#include "wprotocol.h"

/*
 * iCE5 FPGA on the shared SPI1 bus (chip select FPGA_CS); the PIC is the
 * SPI host and streams the configuration bitstream in SPI-slave mode.
 *
 * Send and forget: sent OK = CDONE high.
 */

#define FPGA_UPLOAD_STATUS_IDLE         WP_FPGA_STATUS_IDLE
#define FPGA_UPLOAD_STATUS_LOADING      WP_FPGA_STATUS_LOADING
#define FPGA_UPLOAD_STATUS_DONE         WP_FPGA_STATUS_DONE
#define FPGA_UPLOAD_STATUS_BUS_FAILED   WP_FPGA_STATUS_BUS_FAILED
#define FPGA_UPLOAD_STATUS_CDONE_FAILED WP_FPGA_STATUS_CDONE_FAILED

void fpga_park(void);
void fpga_prog_load(void);
bool fpga_is_cdone(void);
bool fpga_is_uploading(void);
bool fpga_upload_failed(void);
u8 fpga_upload_status(void);

#endif /* FPGA_H */
