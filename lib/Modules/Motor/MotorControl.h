#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Motor/MotorDriver.h>
#include <EEPROM_config.h>

class MotorControl
{
public:
    MotorControl(MotorDriver &rightSide, MotorDriver &leftSide);

    void forward();
    void backward();
    void turnRight(int speed);
    void turnLeft(int speed);
    void stop();

    void setSpeed(int leftSpeed, int rightSpeed);
    void setSpeedAndDirection(int speed, int direction);

private:
    MotorDriver &_rightSide;
    MotorDriver &_leftSide;
    int _currentSpeed;
    int _maxSpeed;
    int _backwardLimit;
    float _turnFactor;
    float calculateTurnFactor();
    static EEPROMConfig _eepromConfig;
};

#endif