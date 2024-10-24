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

    const UBaseType_t _taskPriority = 8;
    const uint32_t _taskStackSize = 4096;
    MotorDriver &_rightMotor;
    MotorDriver &_leftMotor;
    MotorControl _motorControl;

    int _currentSpeed;
};

#endif