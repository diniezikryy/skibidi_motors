#include "motor_control.h"

static float current_pwm_freq = DEFAULT_PWM_FREQ;

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

    // Set up PWM for both motors
    setup_pwm(MOTOR1_PWM_PIN, current_pwm_freq, DEFAULT_DUTY_CYCLE);
    setup_pwm(MOTOR2_PWM_PIN, current_pwm_freq, DEFAULT_DUTY_CYCLE);
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
    set_motor_speed(MOTOR1_PWM_PIN, duty_cycle);
    set_motor_speed(MOTOR2_PWM_PIN, duty_cycle);
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
        set_motor_speed(MOTOR2_PWM_PIN, duty_cycle);
    }
}