#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Motor/MotorDriver.h>

class MotorControl
{
public:
    MotorControl(MotorDriver &rightSide, MotorDriver &leftSide);
    void setSpeed(int rightSpeed, int leftSpeed);
private:
    MotorDriver &_rightSide;
    MotorDriver &_leftSide;
    int _maxSpeed;
    int _backwardLimit;
     int _currentRightSpeed;
    int _currentLeftSpeed;
    int _targetRightSpeed;
    int _targetLeftSpeed;
    int _step;
};

#endif