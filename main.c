#include <stdio.h>
#include "pico/stdlib.h"
#include "motor_control.h"
#include "ir_encoder.h"
#include "pid_control.h"
#include "ultrasonic.h"

#define OBSTACLE_DISTANCE 20.0f    // Increased detection range
#define TURN_DURATION_MS 1000  

void turn_right(void) {
    // Stop completely first
    set_individual_motor_speed(1, 0.0f);
    set_individual_motor_speed(2, 0.0f);
    sleep_ms(200);  // Longer stop
    
    // Make a sharper turn
    set_motor_direction(FORWARD);
    set_individual_motor_speed(1, 0.9f);     // Left motor full
    set_individual_motor_speed(2, 0.0f);    // Right motor backward
    
    sleep_ms(1100);  // Turn longer
    
    // Full stop before resuming
    set_individual_motor_speed(1, 0.0f);
    set_individual_motor_speed(2, 0.0f);
    sleep_ms(200);
    
    // Resume forward
    set_motor_direction(FORWARD);
    set_individual_motor_speed(1, DEFAULT_DUTY_CYCLE);
    set_individual_motor_speed(2, DEFAULT_DUTY_CYCLE);
}

int main() {
    stdio_init_all();
    sleep_ms(4000);
    
    init_motor_pins();
    init_encoders();
    init_pid(); 
    // setup_ultrasonic();
    
    set_motor_direction(FORWARD);
    // bool turning = false;
    
    while (true) {
        float distance = getCm();
        printf("Distance: %.2f cm\n", distance);  // Debug print
        
        // Add validity check for distance
        // if (distance > 0 && distance < OBSTACLE_DISTANCE && !turning) {  // Add upper limit check
        //     printf("Obstacle detected at %.2f cm! Turning right...\n", distance);
        //     turning = true;
        //     turn_right();
        //     turning = false;
        //     sleep_ms(500);
        // }
        
        // if (!turning) {
        //     update_encoder_data();
        //     update_pid();
            
        //     EncoderData* left = get_left_encoder_data();
        //     EncoderData* right = get_right_encoder_data();
        //     printf("Left: %.2f cm/s, Right: %.2f cm/s\n", 
        //            left->speed_cm_s, right->speed_cm_s);
        // }
        
        // sleep_ms(100);  // Add delay between readings
        
        update_encoder_data();
        update_pid();
    }
    
    return 0;
}