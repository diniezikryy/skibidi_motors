#include "pid_control.h"

static struct {
    float previous_error;
    float integral;
} pid_data = {0};

void init_pid(void) {
    reset_pid();
}

void reset_pid(void) {
    pid_data.previous_error = 0.0f;
    pid_data.integral = 0.0f;
}

void update_pid(float target_speed) {
    EncoderData* left = get_left_encoder_data();
    EncoderData* right = get_right_encoder_data();
    
    float error = left->speed_cm_s - right->speed_cm_s;
    
    float P = KP * error;
    pid_data.integral += error;
    float I = KI * pid_data.integral;
    float D = KD * (error - pid_data.previous_error);
    
    float correction = P + I + D;
    
    float base_duty = DEFAULT_DUTY_CYCLE;
    set_individual_motor_speed(1, base_duty - correction);
    set_individual_motor_speed(2, base_duty + correction);
    
    pid_data.previous_error = error;
}