#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// First Motor Pins (Left Motor)
#define MOTOR1_PWM_PIN 2
#define MOTOR1_DIR_PIN1 1
#define MOTOR1_DIR_PIN2 0

// Second Motor Pins (Right Motor)
#define MOTOR2_PWM_PIN 3
#define MOTOR2_DIR_PIN1 4
#define MOTOR2_DIR_PIN2 5

// PWM configuration for floor operation
#define DEFAULT_PWM_FREQ 125.0f
#define DEFAULT_DUTY_CYCLE 0.7f     // Reduce from 0.8
#define MIN_DUTY_CYCLE 0.4f         // Keep as is
#define MAX_DUTY_CYCLE 0.9f         // Reduce from 0.95

// Increased compensation for right motor
#define LEFT_MOTOR_COMPENSATION 1.1f  // Add this new compensation factor
#define RIGHT_MOTOR_COMPENSATION 1.0f  // Modify existing to be explicit

// Direction definitions
typedef enum {
    FORWARD,
    BACKWARD,
    STOP
} MotorDirection;

// Function declarations
void setup_pwm(uint gpio, float freq, float duty_cycle);
void init_motor_pins(void);
void set_motor_direction(MotorDirection direction);
void set_motor_speed(uint motor_pwm_pin, float duty_cycle);
void set_both_motor_speeds(float duty_cycle);
void stop_motors(void);
void start_motors(void);
void set_pwm_frequency(float new_freq);
void set_individual_motor_speed(uint motor_number, float duty_cycle);

#endif // MOTOR_CONTROL_H