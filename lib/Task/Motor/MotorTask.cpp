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
    xTaskCreate(runTask, "MotorControlTask", 3048, this, 8, NULL);
}

void MotorTask::setSpeedAndDirection(int newSpeed, int newDirection) {
    speed = newSpeed;
    direction = newDirection;
}

void MotorTask::runTask(void *pvParameters)
{
    MotorTask *motorTask = static_cast<MotorTask *>(pvParameters);
    
    int currentSpeed = 0;
    
    while (true)
    {
        int targetSpeed = speed;

        if (currentSpeed < targetSpeed) {
            currentSpeed += 20;
        } else if (currentSpeed > targetSpeed) {
            currentSpeed -= 20;
        }

        if (currentSpeed == 0) {
            motorTask->_motorControl.stop();
        } else {
            motorTask->_motorControl.setDirectionAndSpeed(currentSpeed, direction);
        }

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}