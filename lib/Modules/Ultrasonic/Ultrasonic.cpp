#include <Ultrasonic/Ultrasonic.h>

#define MAX_DISTANCE 400

Ultrasonic::Ultrasonic(int triggerPin, int echoPin) 
    : _trigger_pin(triggerPin), _echo_pin(echoPin) {}

void Ultrasonic::begin() {
    pinMode(_trigger_pin, OUTPUT);
    pinMode(_echo_pin, INPUT);
}

long Ultrasonic::getDistance() {
    digitalWrite(_trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigger_pin, LOW);

    long duration = pulseIn(_echo_pin, HIGH, 30000);

    if (duration == 0) {
        return -1;
    }

    long distance = (duration * 0.034) / 2;

    return distance;
}