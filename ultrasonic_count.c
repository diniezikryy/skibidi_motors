#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Pin definitions
#define TRIGGER_PIN 26
#define ECHO_PIN 27
#define NUM_READINGS 150

// Function prototypes
float take_measurement(void);
float calculate_mean(float readings[], int size);
float calculate_variance(float readings[], float mean, int size);

// Global array to store readings
float distance_readings[NUM_READINGS];

// Function to take a single ultrasonic measurement
float take_measurement() {
    uint32_t start_time, end_time;
    float distance;
    
    // Send trigger pulse
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);
    
    // Wait for echo pin to go high
    while (gpio_get(ECHO_PIN) == 0) {
        tight_loop_contents();
    }
    start_time = time_us_32();
    
    // Wait for echo pin to go low
    while (gpio_get(ECHO_PIN) == 1) {
        tight_loop_contents();
    }
    end_time = time_us_32();
    
    // Calculate distance in cm
    // Distance = (Time × Speed of Sound) ÷ 2
    // Speed of Sound = 343 meters/second = 0.0343 cm/microsecond
    distance = (float)(end_time - start_time) * 0.0343 / 2.0;
    
    return distance;
}

// Function to calculate mean of readings
float calculate_mean(float readings[], int size) {
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += readings[i];
    }
    return sum / size;
}

// Function to calculate variance
float calculate_variance(float readings[], float mean, int size) {
    float sum_squared_diff = 0.0;
    for (int i = 0; i < size; i++) {
        float diff = readings[i] - mean;
        sum_squared_diff += diff * diff;
    }
    return sum_squared_diff / size;
}

// Function to calculate variance
float calculate_total(float readings[], int size) {
    float total = 0.0;
    for (int i = 0; i < size; i++) {
        total += i;
    }
    return total;
}

int main() {
    // Initialize stdio
    stdio_init_all();
    sleep_ms(5000);
    printf("starting event!\n");
    
    // Configure GPIO pins
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    
    printf("Starting data collection...\n");
    
    while (1) {
        printf("started !\n");
        // Collect readings
        for (int i = 0; i < NUM_READINGS; i++) {
            // Take measurement
            distance_readings[i] = take_measurement();
            
            // Print current reading
            printf("Reading %d: %.2f cm\n", i + 1, distance_readings[i]);
            
            // Wait between readings
            sleep_ms(100);  // 100ms delay between readings
        }
        printf("ended !\n");
        break;
    }
    
    // Calculate statistics
    float total = calculate_total(distance_readings, NUM_READINGS);
    float mean = calculate_mean(distance_readings, NUM_READINGS);
    float variance = calculate_variance(distance_readings, mean, NUM_READINGS);
    float std_deviation = sqrt(variance);
    
    // Print results
    printf("\nStatistical Analysis:\n");
    printf("Total: %.2f\n", total);
    printf("Mean Distance: %.2f cm\n", mean);
    printf("Variance: %.2f\n", variance);
    printf("Standard Deviation: %.2f\n", std_deviation);
    printf("Error Rate: %.2f\n", mean - 38.75);
    
    
    // Print all readings for verification or further analysis
    printf("\nAll Readings:\n");
    for (int i = 0; i < NUM_READINGS; i++) {
        printf("%.2f", distance_readings[i]);
        if (i < NUM_READINGS - 1) {
            printf(", ");
        }
        if ((i + 1) % 10 == 0) {  // New line every 10 readings
            printf("\n");
        }
    }
    
    return 0;
}