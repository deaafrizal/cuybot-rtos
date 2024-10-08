#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#define PWM_STANDBY 8

#include <Arduino.h>

class MotorDriver
{
public:
    MotorDriver(uint8_t pinA, uint8_t pinB);

    void motorGo(uint8_t pwm);
    void motorRev(uint8_t pwm);
    void motorStop();
    void motorBrake();

private:
    const uint8_t _pwm_standby_pin = PWM_STANDBY;
    uint8_t _pinA;
    uint8_t _pinB;
    uint8_t _pwm;
    uint8_t _maxPwm;
};

#endif
