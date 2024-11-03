#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include "motor_control.h"
#include "ir_encoder.h"

// PID constants
#define KP 0.005f    // Proportional gain
#define KI 0.001f    // Integral gain
#define KD 0.001f    // Derivative gain

// Function declarations
void init_pid(void);
void update_pid(float target_speed);
void reset_pid(void);

#endif // PID_CONTROL_H