#include <stdio.h>
#include "pico/stdlib.h"
#include "motor_control.h"
#include "ir_encoder.h"
#include "pid_control.h"
#include "ultrasonic.h"

#define OBSTACLE_DISTANCE 25.0f // Distance in cm to trigger turn
#define TURN_DURATION_MS 1000  // 500ms for turn

void turn_right(void) {
    // First stop both motors briefly
    set_individual_motor_speed(1, 0.0f);
    set_individual_motor_speed(2, 0.0f);
    sleep_ms(100);  // Brief pause
    
    // Make a sharp right turn:
    // Left motor forward at high power
    // Right motor backward or stopped
    set_motor_direction(FORWARD);
    set_individual_motor_speed(1, 0.9f);    // Left motor strong forward
    //set_motor_direction(BACKWARD);
    set_individual_motor_speed(2, 0.0f);    // Right motor backward
    
    sleep_ms(TURN_DURATION_MS);
    
    // Return to forward motion
    set_motor_direction(FORWARD);
    set_individual_motor_speed(1, DEFAULT_DUTY_CYCLE);
    set_individual_motor_speed(2, DEFAULT_DUTY_CYCLE);
}

int main() {
    stdio_init_all();
    sleep_ms(4000);
    // printf("Initializing...\n");
    
    init_motor_pins();
    init_encoders();
    init_pid();
    setup_ultrasonic();
    
    // printf("Starting motors...\n");
    set_motor_direction(FORWARD);
    
    bool turning = false;  // Flag to track turn state
    
    while (true) {
        float distance = getCm();
        
        if (distance > 0 && distance < OBSTACLE_DISTANCE && !turning) {
            // printf("Obstacle detected at %.2f cm! Turning right...\n", distance);
            turning = true;  // Set turning flag
            turn_right();
            turning = false;  // Reset turning flag
            sleep_ms(500);   // Brief delay after turn
        }
        
        if (!turning) {  // Only update PID when not turning
            update_encoder_data();
            update_pid();
        }
        
        EncoderData* left = get_left_encoder_data();
        EncoderData* right = get_right_encoder_data();
        
        // printf("Distance: %.2f cm, Left: %.2f cm/s, Right: %.2f cm/s\n", 
        //        distance, left->speed_cm_s, right->speed_cm_s);
        
        sleep_ms(100);
    }
    
    return 0;
}