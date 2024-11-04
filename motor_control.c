#include "motor_control.h"

static float current_pwm_freq = DEFAULT_PWM_FREQ;
static float ramp_duty_cycle = 0.0f;

void setup_pwm(uint gpio, float freq, float duty_cycle) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    float clock_freq = 125.0f;
    uint32_t divider = clock_freq / (freq * 65536);
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_gpio_level(gpio, (uint16_t)(duty_cycle * 65536));
    pwm_set_enabled(slice_num, true);
}

void init_motor_pins() {
    // Initialize direction pins
    gpio_init(MOTOR1_DIR_PIN1);
    gpio_init(MOTOR1_DIR_PIN2);
    gpio_init(MOTOR2_DIR_PIN1);
    gpio_init(MOTOR2_DIR_PIN2);
    
    gpio_set_dir(MOTOR1_DIR_PIN1, GPIO_OUT);
    gpio_set_dir(MOTOR1_DIR_PIN2, GPIO_OUT);
    gpio_set_dir(MOTOR2_DIR_PIN1, GPIO_OUT);
    gpio_set_dir(MOTOR2_DIR_PIN2, GPIO_OUT);

    // Set up PWM for both motors - start at 0 duty cycle
    setup_pwm(MOTOR1_PWM_PIN, current_pwm_freq, 0.0f);
    setup_pwm(MOTOR2_PWM_PIN, current_pwm_freq, 0.0f);
    
    // Ensure both motors are stopped initially
    stop_motors();
}

// void start_motors(void) {
//     const float RAMP_STEP = 0.05f;  // Increase by 5% each step
//     const int RAMP_DELAY_MS = 50;   // 50ms between steps
    
//     // Ensure motors are in forward direction
//     set_motor_direction(FORWARD);
    
//     // Gradually ramp up both motors together
//     for(float duty = 0.0f; duty <= DEFAULT_DUTY_CYCLE; duty += RAMP_STEP) {
//         // set_both_motor_speeds(duty);
//         set_individual_motor_speed(1, 0.75f);  // Left motor at 30%
//         set_individual_motor_speed(2, 0.6f);  // Right motor at 40%
//         sleep_ms(RAMP_DELAY_MS);
//     }
// }

void start_motors(void) {
    const float RAMP_STEP = 0.02f;  // Smaller steps (2%)
    const int RAMP_DELAY_MS = 50;   // 50ms between steps
    
    // Ensure motors are in forward direction
    set_motor_direction(FORWARD);
    
    // Gradually ramp up both motors together from MIN to DEFAULT
    for(float duty = MIN_DUTY_CYCLE; duty <= DEFAULT_DUTY_CYCLE; duty += RAMP_STEP) {
        set_both_motor_speeds(duty);  // Same duty cycle for both motors
        sleep_ms(RAMP_DELAY_MS);
    }
}

void set_motor_direction(MotorDirection direction) {
    switch(direction) {
        case FORWARD:
            gpio_put(MOTOR1_DIR_PIN1, 1);
            gpio_put(MOTOR1_DIR_PIN2, 0);
            gpio_put(MOTOR2_DIR_PIN1, 1);
            gpio_put(MOTOR2_DIR_PIN2, 0);
            break;
            
        case BACKWARD:
            gpio_put(MOTOR1_DIR_PIN1, 0);
            gpio_put(MOTOR1_DIR_PIN2, 1);
            gpio_put(MOTOR2_DIR_PIN1, 0);
            gpio_put(MOTOR2_DIR_PIN2, 1);
            break;
            
        case STOP:
            gpio_put(MOTOR1_DIR_PIN1, 0);
            gpio_put(MOTOR1_DIR_PIN2, 0);
            gpio_put(MOTOR2_DIR_PIN1, 0);
            gpio_put(MOTOR2_DIR_PIN2, 0);
            break;
    }
}

void set_motor_speed(uint motor_pwm_pin, float duty_cycle) {
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 1.0f) duty_cycle = 1.0f;
    pwm_set_gpio_level(motor_pwm_pin, (uint16_t)(duty_cycle * 65536));
}

void set_both_motor_speeds(float duty_cycle) {
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 1.0f) duty_cycle = 1.0f;
    
    // Set both motors exactly the same
    set_motor_speed(MOTOR1_PWM_PIN, duty_cycle);
    set_motor_speed(MOTOR2_PWM_PIN, duty_cycle);
    
    ramp_duty_cycle = duty_cycle;
}

void stop_motors(void) {
    set_motor_direction(STOP);
    set_both_motor_speeds(0.0f);
}

void set_pwm_frequency(float new_freq) {
    current_pwm_freq = new_freq;
    setup_pwm(MOTOR1_PWM_PIN, new_freq, DEFAULT_DUTY_CYCLE);
    setup_pwm(MOTOR2_PWM_PIN, new_freq, DEFAULT_DUTY_CYCLE);
}

void set_individual_motor_speed(uint motor_number, float duty_cycle) {
    if (motor_number == 1) {
        set_motor_speed(MOTOR1_PWM_PIN, duty_cycle);
    } else if (motor_number == 2) {
        // Apply compensation to right motor
        float compensated_duty = duty_cycle * RIGHT_MOTOR_COMPENSATION;
        if (compensated_duty > MAX_DUTY_CYCLE) compensated_duty = MAX_DUTY_CYCLE;
        set_motor_speed(MOTOR2_PWM_PIN, compensated_duty);
    }
}

// float calculate_duty_cycle_for_speed(float target_speed_cm_s) {
//     // Constrain target speed
//     if (target_speed_cm_s < MIN_SPEED_CM_S) target_speed_cm_s = MIN_SPEED_CM_S;
//     if (target_speed_cm_s > MAX_SPEED_CM_S) target_speed_cm_s = MAX_SPEED_CM_S;
    
//     // Linear mapping from speed to duty cycle
//     float speed_range = MAX_SPEED_CM_S - MIN_SPEED_CM_S;
//     float duty_range = MAX_DUTY_CYCLE - MIN_DUTY_CYCLE;
    
//     float duty_cycle = MIN_DUTY_CYCLE + 
//         (target_speed_cm_s - MIN_SPEED_CM_S) * duty_range / speed_range;
    
//     return duty_cycle;
// }