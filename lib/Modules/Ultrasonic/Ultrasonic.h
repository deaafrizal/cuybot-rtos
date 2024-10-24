#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <Arduino.h>

class Ultrasonic {
public:
    Ultrasonic(int triggerPin, int echoPin);
    void begin();
    long getDistance();

private:
    const int _trigger_pin;
    const int _echo_pin;
};

#endif