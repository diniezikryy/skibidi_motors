#include <stdio.h>
#include "pico/stdlib.h"
#include "motor_control.h"
#include "ir_encoder.h"
#include "pid_control.h"

int main() {
    stdio_init_all();
    sleep_ms(5000);  // Initial delay for setup
    
    init_motor_pins();
    init_encoders();
    init_pid();
    
    set_motor_direction(FORWARD);
    
    float target_speed = 40.0f;  // Target speed in cm/s
    
    while (true) {
        update_encoder_data();
        update_pid(target_speed);
        
        EncoderData* left = get_left_encoder_data();
        EncoderData* right = get_right_encoder_data();
        
        printf("Left: %.2f cm/s, Right: %.2f cm/s\n", 
               left->speed_cm_s, right->speed_cm_s);
        
        sleep_ms(100);  // Update interval
    }
    
    return 0;
}