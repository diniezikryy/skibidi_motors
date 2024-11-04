#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define TRIG_PIN 18
#define ECHO_PIN 19
#define OFFSET -0.9

typedef struct {
    float Q;          // Process noise covariance
    float R;          // Base measurement noise covariance
    float X;          // State estimate
    float P;          // Estimation error covariance
    float K;          // Kalman gain
    float offset;     // Calibration offset
    bool initialized; 
} KalmanFilter;

// Initialize Kalman filter with calibration offset,, initialisd ONCE
static KalmanFilter filter = {
    .P = 0.7,    // Error Covarience: confidence (1 = not confident, 0 = confident)
    .Q = 0.05,    // Process Noise: how much noise
    .R = 0.15,    // Measurment Noise: varience 
    .X = 0.0,    // estimated value
    .initialized = false,
    .offset = OFFSET,  // Add calibration offset
};

float kalman_update(KalmanFilter* filter, float raw_measurement);
void setup_ultrasonic();
uint64_t getPulse();
float getCm();
float getInch();

#endif