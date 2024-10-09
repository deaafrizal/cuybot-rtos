#include <Motor/MotorTask.h>

extern int speed;
extern int direction;

MotorTask::MotorTask(uint8_t rightMotorPinA, uint8_t rightMotorPinB, uint8_t leftMotorPinA, uint8_t leftMotorPinB)
    : _rightMotor(rightMotorPinA, rightMotorPinB), 
      _leftMotor(leftMotorPinA, leftMotorPinB),   
      _motorControl(_rightMotor, _leftMotor)
{
    
}

void MotorTask::startTask()
{
    xTaskCreate(runTask, "MotorControlTask", 4096, this, 8, NULL);
}

void MotorTask::runTask(void *pvParameters)
{
    MotorTask *motorTask = static_cast<MotorTask *>(pvParameters);

    while (true)
    {
        int currentSpeed = speed;
        int currentDirection = direction;

        motorTask->_motorControl.setDirectionAndSpeed(currentSpeed, currentDirection);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}