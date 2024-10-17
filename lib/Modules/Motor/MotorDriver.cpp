#include <Motor/MotorDriver.h>

MotorDriver::MotorDriver(uint8_t pinA, uint8_t pinB) : _maxPwm(255)
{
    _pinA = pinA;
    _pinB = pinB;
    
    digitalWrite(_pwm_standby_pin, HIGH);
    
    pinMode(_pinA, OUTPUT);
    pinMode(_pinB, OUTPUT);
}

void MotorDriver::forward(uint8_t pwm)
{
    _pwm = constrain(pwm, 0, _maxPwm);
    // normal
    analogWrite(_pinA, _pwm);
    analogWrite(_pinB, 0);

    // reverse
    //analogWrite(_pinA, 0);
    //analogWrite(_pinB, _pwm);  
}

void MotorDriver::backward(uint8_t pwm)
{
    _pwm = constrain(pwm, 0, _maxPwm);
    //normal
    analogWrite(_pinA, 0);
    analogWrite(_pinB, _pwm);  

    // reverse
    //analogWrite(_pinA, _pwm);
    //analogWrite(_pinB, 0);
}

void MotorDriver::stop()
{
    analogWrite(_pinA, 0);
    analogWrite(_pinB, 0);
}

/*
  -----NORMAL MOTOR WIRING-----
  IN1 LO + IN2 LO = standby
  IN1 LO + IN2 HI = backward / reverse
  IN1 HI + IN2 LO = forward
  IN1 HI + IN2 HI = brake

  ----REVERSED MOTOR WIRING----
  IN1 LO + IN2 HI = forward
  IN1 HI + IN1 LO = backward / reverse
**/
