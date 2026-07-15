#ifndef SENSOR_SERVICE_H
#define SENSOR_SERVICE_H

#include <stdbool.h>

#include "utils.h"

/* Initialize the shared ADC core and register the raw sensor reporters. */
void sensor_service_init(void);

/* Read the two servo channels while holding the shared ADC-core lock. */
bool sensor_service_read_servo(u16 *position, u16 *sense);

#endif /* SENSOR_SERVICE_H */
