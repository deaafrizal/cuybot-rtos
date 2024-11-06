#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <Arduino.h>

class MotorDriver
{
public:
    MotorDriver(uint8_t pinA, uint8_t pinB);
    void begin();
    void forward(uint8_t pwm);
    void backward(uint8_t pwm);
    void stop();

private:
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _pwm;
    uint8_t _maxPwm;
};

#endif
