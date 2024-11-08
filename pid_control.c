#include "pid_control.h"
#include <math.h>

static struct {
    float previous_error;
    float integral;
    float previous_correction;
} pid_data = {0};

void init_pid(void) {
    set_motor_direction(FORWARD);
    set_individual_motor_speed(1, DEFAULT_DUTY_CYCLE);
    set_individual_motor_speed(2, DEFAULT_DUTY_CYCLE);
    pid_data.previous_error = 0.0f;
    pid_data.integral = 0.0f;
    pid_data.previous_correction = 0.0f;
}

void update_pid(void) {
    EncoderData* left = get_left_encoder_data();
    EncoderData* right = get_right_encoder_data();
    
    // Calculate error (positive when right is faster)
    float error = right->speed_cm_s - left->speed_cm_s;
    
    // Apply deadband
    if(fabs(error) < DEADBAND) {
        error = 0.0f;
    }
    
    // Update integral with limits to prevent windup
    pid_data.integral += error;
    if(pid_data.integral > 10.0f) pid_data.integral = 10.0f;
    if(pid_data.integral < -10.0f) pid_data.integral = -10.0f;
    
    // Reset integral if error changes direction
    if((error > 0 && pid_data.previous_error < 0) || 
       (error < 0 && pid_data.previous_error > 0)) {
        pid_data.integral = 0;
    }
    
    // Calculate P, I, and D terms
    float P = KP * error;
    float I = KI * pid_data.integral;
    float D = KD * (error - pid_data.previous_error);
    
    // Calculate total correction
    float correction = P + I + D;
    
    // Limit correction change rate to prevent sudden changes
    float correction_diff = correction - pid_data.previous_correction;
    if(fabs(correction_diff) > 0.05f) {
        correction = pid_data.previous_correction + 
                    (correction_diff > 0 ? 0.05f : -0.05f);
    }
    
    // Apply corrections to motor powers
    float left_power = DEFAULT_DUTY_CYCLE + correction;
    float right_power = DEFAULT_DUTY_CYCLE - correction;
    
    // Limit powers
    if(left_power < 0.4f) left_power = 0.4f;
    if(left_power > 0.95f) left_power = 0.95f;
    if(right_power < 0.4f) right_power = 0.4f;
    if(right_power > 0.95f) right_power = 0.95f;
    
    set_individual_motor_speed(1, left_power);
    set_individual_motor_speed(2, right_power);
    
    // Store values for next iteration
    pid_data.previous_error = error;
    pid_data.previous_correction = correction;
    
    // Debug output
    // printf("Error: %.2f, P: %.3f, I: %.3f, D: %.3f, L: %.3f, R: %.3f\n", 
    //        error, P, I, D, left_power, right_power);
}