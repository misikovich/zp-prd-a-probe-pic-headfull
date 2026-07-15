#ifndef SERVO_SERVICE_H
#define SERVO_SERVICE_H

/* Park the servo power, register protocol handlers/reporters, and start the
   sampling/self-test task. Call after sensor_service_init(). */
void servo_service_init(void);

#endif /* SERVO_SERVICE_H */
