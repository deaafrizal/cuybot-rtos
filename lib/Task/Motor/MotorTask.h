#ifndef MOTORTASK_H
#define MOTORTASK_H

#include <Motor/MotorControl.h>
#include <Motor/MotorDriver.h>

class MotorTask
{
public:
    MotorTask(MotorDriver &rightMotor, MotorDriver &leftMotor);
    ~MotorTask();
    
    void startTask();

private:
    static void runTask(void *pvParameters);

    MotorDriver &_rightMotor;
    MotorDriver &_leftMotor;
    MotorControl _motorControl;

    int _currentSpeed;
};

#endif