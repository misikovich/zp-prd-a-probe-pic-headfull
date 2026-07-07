#ifndef FPGA_H
#define FPGA_H

#include <stdbool.h>

#include "utils.h"

/*
 * iCE5 FPGA on the shared SPI1 bus (chip select FPGA_CS); the PIC is the
 * SPI host and streams the configuration bitstream in SPI-slave mode.
 *
 * Send and forget: sent OK = CDONE high.
 */

#define FPGA_UPLOAD_STATUS_IDLE         0u
#define FPGA_UPLOAD_STATUS_LOADING      1u
#define FPGA_UPLOAD_STATUS_DONE         2u
#define FPGA_UPLOAD_STATUS_BUS_FAILED   3u  /* SPI1 bus not claimed in time */
#define FPGA_UPLOAD_STATUS_CDONE_FAILED 4u

void fpga_park(void);
void fpga_prog_load(void);
bool fpga_is_cdone(void);
bool fpga_is_uploading(void);
bool fpga_upload_failed(void);
u8 fpga_upload_status(void);

#endif /* FPGA_H */
