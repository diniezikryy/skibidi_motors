#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "ultrasonic.h"

const uint32_t timeout = 26100;  // Timeout in microseconds

// Update Kalman filter with new measurement
float kalman_update(KalmanFilter* filter, float raw_measurement) {
    // Apply calibration correction first
    float calibrated_measurement = raw_measurement + filter->offset;
    
    if (!filter->initialized) {
        filter->X = calibrated_measurement;
        filter->initialized = true;
        return filter->X;
    }

    // Prediction step
    float predicted_X = filter->X;
    float predicted_P = filter->P + filter->Q;

    // Update step
    filter->K = predicted_P / (predicted_P + filter->R);  // Use fixed R without error rate adjustment
    filter->X = predicted_X + filter->K * (calibrated_measurement - predicted_X);
    filter->P = (1 - filter->K) * predicted_P;

    return filter->X;
}
 
uint64_t getPulse()
{
    // Start with trigger pin low
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);  // Short delay to ensure pin is low
    
    // Send trigger pulse
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);
    
    uint64_t width = 0;
    absolute_time_t startTime = get_absolute_time();
    // Wait for echo to go high
    while (gpio_get(ECHO_PIN) == 0) {
        if (absolute_time_diff_us(startTime, get_absolute_time()) > timeout) {
            return 0;  // Timeout waiting for echo to start
        }
        tight_loop_contents();
    }

    // Measure the pulse width
    startTime = get_absolute_time();
    while (gpio_get(ECHO_PIN) == 1) {
        width++;    
        sleep_us(1);
        if (width > timeout) return 0;
    }
    return absolute_time_diff_us(startTime, get_absolute_time());
}

float getCm()
{
    uint64_t pulseLength = getPulse();
    float measured_length = (float)pulseLength / 29 / 2; // convert pulse length to cm

    return kalman_update(&filter, measured_length);
}

float getInch()
{
    uint64_t pulseLength = getPulse();
    float measured_length = (float)pulseLength / 74 / 2; // convert pulse length to inches

    return kalman_update(&filter, measured_length);
}

void setup_ultrasonic() 
{
    // setup ultrasonic pins
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    // Initialize trigger pin to low
    gpio_put(TRIG_PIN, 0);

    printf("Ultrasonic Setup complete!\n");
}
 
// int main()
// {
//     stdio_init_all();
//     sleep_ms(3000);
//     printf("Ultrasonic sensor started!\n");

//     setup_ultrasonic();
 
//     while (true) { 
//         float distance = getCm();
//         if (distance < 0) {
//             printf("Error reading sensor\n");
//         } else {
//             printf("Measured distance: %.2f cm\n", distance);
//         }
        
//         sleep_ms(100);
//     }
// }
