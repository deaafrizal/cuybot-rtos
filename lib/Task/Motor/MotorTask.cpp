#include <Motor/MotorTask.h>

extern int motorSpeed;
extern int motorDirection;

MotorTask::MotorTask(MotorDriver &rightMotor, MotorDriver &leftMotor)
    : _rightMotor(rightMotor), _leftMotor(leftMotor), _motorControl(_rightMotor, _leftMotor)
{
}

void MotorTask::startTask()
{
    xTaskCreate(runTask, "MotorControlTask", 3048, this, 6, NULL);
}

void MotorTask::setDirection(int newDirection) {
    motorDirection = newDirection;
}

void MotorTask::setSpeed(int newSpeed) {
    motorSpeed = newSpeed;
}

void MotorTask::runTask(void *pvParameters)
{
    MotorTask *motorTask = static_cast<MotorTask *>(pvParameters);
    
    int currentSpeed = 0;
    
    while (true)
    {
        int targetSpeed = motorSpeed;

        if (currentSpeed < targetSpeed) {
            currentSpeed += 25;
        } else if (currentSpeed > targetSpeed) {
            currentSpeed -= 25;
        }

        if (currentSpeed == 0) {
            motorTask->_motorControl.stop();
        } else {
            motorTask->_motorControl.setSpeedAndDirection(currentSpeed, motorDirection);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}