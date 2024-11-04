#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include <math.h>
#include "motor_control.h"
#include "ir_encoder.h"

#define KP 0.15f          // Reduced for gentler corrections
#define KI 0.05f          // Much smaller integral term
#define KD 0.08f          // Increased damping to reduce oscillation
#define DEADBAND 1.5f     // Wider deadband to ignore small differences

#define LEFT_BIAS 1.00f  // Adjust this value based on testing


void init_pid(void);
void update_pid(void);

#endif // PID_CONTROL_H