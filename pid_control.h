#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include <math.h>
#include "motor_control.h"
#include "ir_encoder.h"

// Increased proportional gain for stronger correction
#define KP 0.6f            // Doubled from 0.02 for stronger correction
#define KI 0.2f           // Small integral term to handle persistent error
#define KD 0.1f            // Added derivative term to prevent oscillation

// Higher deadband since we're using higher speeds
#define DEADBAND 1.0f       // Reduced to make it more responsive

void init_pid(void);
void update_pid(void);

#endif // PID_CONTROL_H