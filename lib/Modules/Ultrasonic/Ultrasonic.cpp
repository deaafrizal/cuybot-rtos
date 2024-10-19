#include <Ultrasonic/Ultrasonic.h>
#include <Arduino.h>
#include <NewPing.h>

#define MAX_DISTANCE 200

Ultrasonic::Ultrasonic()
    : sonar(_trigger_pin, _echo_pin, MAX_DISTANCE) {}

void Ultrasonic::begin() {
    sonar.ping();
}

long Ultrasonic::getDistance() {
    unsigned int distance = sonar.ping_cm();

    if (distance == 0) {
        return MAX_DISTANCE;
    }
    
    return distance;
}