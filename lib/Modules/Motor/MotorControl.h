#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Motor/MotorDriver.h>

class MotorControl
{
public:
    MotorControl(MotorDriver &rightMotor, MotorDriver &leftMotor);

    void forward();
    void backward();
    void turnRight();
    void turnLeft();

    void brake();
    void stop();
    
    void setSpeed(int speed);
    void setDirectionAndSpeed(int speed, char direction);

private:
    MotorDriver &_rightMotor;
    MotorDriver &_leftMotor;
    int _currentSpeed;
    int _maxSpeed;
    float _turnFactor;
};

#endif