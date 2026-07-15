#ifndef SERVO_SERVICE_H
#define SERVO_SERVICE_H

/* Park servo power, register manual-position/telemetry protocol handlers,
   and start the power-timeout/sampling task. Call after sensor_service_init(). */
void servo_service_init(void);

#endif /* SERVO_SERVICE_H */
