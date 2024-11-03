#ifndef IR_ENCODER_H
#define IR_ENCODER_H

#include "pico/stdlib.h"
#include <stdio.h>

// Encoder pins
#define LEFT_ENCODER_PIN 8
#define RIGHT_ENCODER_PIN 9

// Wheel specifications
#define SLITS_PER_REVOLUTION 20
#define WHEEL_CIRCUMFERENCE_CM 20.4f
#define CM_PER_SLIT (WHEEL_CIRCUMFERENCE_CM / SLITS_PER_REVOLUTION)

typedef struct {
    uint pulse_count;
    uint64_t last_time;
    float rps;           // Revolutions per second
    float speed_cm_s;    // Speed in centimeters per second
} EncoderData;

// Function declarations
void init_encoders(void);
void update_encoder_data(void);
EncoderData* get_left_encoder_data(void);
EncoderData* get_right_encoder_data(void);
float get_average_speed(void);

#endif // IR_ENCODER_H