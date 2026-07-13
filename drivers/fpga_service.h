#ifndef FPGA_SERVICE_H
#define FPGA_SERVICE_H

/* Park the FPGA, register its protocol command/reporters, and create the
   persistent upload worker. No upload is performed at boot. */
void fpga_service_init(void);

#endif /* FPGA_SERVICE_H */
