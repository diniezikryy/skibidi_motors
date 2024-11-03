#include "ir_encoder.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define AVERAGING_WINDOW 5  // Number of readings to average

static EncoderData left_encoder = {0};
static EncoderData right_encoder = {0};

// Add arrays to store historical values
static float left_speeds[AVERAGING_WINDOW] = {0};
static float right_speeds[AVERAGING_WINDOW] = {0};
static int speed_index = 0;

void gpio_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();
    
    if (gpio == LEFT_ENCODER_PIN) {
        left_encoder.pulse_count++;
    } else if (gpio == RIGHT_ENCODER_PIN) {
        right_encoder.pulse_count++;
    }
}

void init_encoders(void) {
    gpio_init(LEFT_ENCODER_PIN);
    gpio_init(RIGHT_ENCODER_PIN);
    gpio_set_dir(LEFT_ENCODER_PIN, GPIO_IN);
    gpio_set_dir(RIGHT_ENCODER_PIN, GPIO_IN);

    gpio_pull_up(LEFT_ENCODER_PIN);
    gpio_pull_up(RIGHT_ENCODER_PIN);

    gpio_set_irq_enabled_with_callback(LEFT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
    gpio_set_irq_enabled(RIGHT_ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true);

    left_encoder.last_time = time_us_64();
    right_encoder.last_time = time_us_64();
}

float calculate_average_speed(float speeds[], int count) {
    float sum = 0;
    for(int i = 0; i < count; i++) {
        sum += speeds[i];
    }
    return sum / count;
}

void update_encoder_data(void) {
    uint64_t current_time = time_us_64();
    float time_diff_sec;

    // Update left encoder
    time_diff_sec = (current_time - left_encoder.last_time) / 1000000.0f;
    if (time_diff_sec >= 0.05f) {  // Reduced from 0.1s to 0.05s for more frequent updates
        // Calculate instantaneous speed
        left_encoder.rps = (float)left_encoder.pulse_count / SLITS_PER_REVOLUTION / time_diff_sec;
        float instant_speed = left_encoder.rps * WHEEL_CIRCUMFERENCE_CM;
        
        // Store in circular buffer
        left_speeds[speed_index] = instant_speed;
        
        // Calculate averaged speed
        left_encoder.speed_cm_s = calculate_average_speed(left_speeds, AVERAGING_WINDOW);
        
        // Reset pulse count and update time
        left_encoder.pulse_count = 0;
        left_encoder.last_time = current_time;
    }

    // Update right encoder
    time_diff_sec = (current_time - right_encoder.last_time) / 1000000.0f;
    if (time_diff_sec >= 0.05f) {
        right_encoder.rps = (float)right_encoder.pulse_count / SLITS_PER_REVOLUTION / time_diff_sec;
        float instant_speed = right_encoder.rps * WHEEL_CIRCUMFERENCE_CM;
        
        // Store in circular buffer
        right_speeds[speed_index] = instant_speed;
        
        // Calculate averaged speed
        right_encoder.speed_cm_s = calculate_average_speed(right_speeds, AVERAGING_WINDOW);
        
        right_encoder.pulse_count = 0;
        right_encoder.last_time = current_time;
    }

    // Update circular buffer index
    speed_index = (speed_index + 1) % AVERAGING_WINDOW;
}

EncoderData* get_left_encoder_data(void) {
    return &left_encoder;
}

EncoderData* get_right_encoder_data(void) {
    return &right_encoder;
}

float get_average_speed(void) {
    return (left_encoder.speed_cm_s + right_encoder.speed_cm_s) / 2.0f;
}