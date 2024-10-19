#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <NewPing.h>

#define TRIGGER_PIN 20
#define ECHO_PIN 21

class Ultrasonic {
public:
    Ultrasonic();

    void begin();
    long getDistance();

private:
    const uint8_t _trigger_pin = TRIGGER_PIN;
    const uint8_t _echo_pin = ECHO_PIN;
    NewPing sonar;

};

#endif