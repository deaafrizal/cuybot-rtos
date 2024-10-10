#ifndef MOTORTASK_H
#define MOTORTASK_H

#include <Motor/MotorControl.h>

class MotorTask
{
public:
    MotorTask(uint8_t rightMotorPinA, uint8_t rightMotorPinB, uint8_t leftMotorPinA, uint8_t leftMotorPinB);
    void setDirection(int direction);
    void startTask();

private:
    static void runTask(void *pvParameters);

    MotorDriver _rightMotor;
    MotorDriver _leftMotor;
    MotorControl _motorControl;
};

#endif