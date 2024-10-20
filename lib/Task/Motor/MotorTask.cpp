#include <Motor/MotorTask.h>

extern int motorSpeed;
extern int motorDirection;

MotorTask::MotorTask(MotorDriver &rightMotor, MotorDriver &leftMotor, MotorControl &motorControl)
    : _rightMotor(rightMotor), _leftMotor(leftMotor), _motorControl(motorControl)
{
}

MotorTask::~MotorTask() {
    //
}

void MotorTask::startTask()
{
    xTaskCreate(runTask, "MotorControlTask", 3248, this, 4, NULL);
}


void MotorTask::setDirection(int newDirection) {
    motorDirection = newDirection;
}

void MotorTask::setSpeed(int newSpeed) {
    motorSpeed = newSpeed;
}

void MotorTask::runTask(void *pvParameters)
{
    MotorTask *self = static_cast<MotorTask *>(pvParameters);
    
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
            self->_motorControl.stop();
        } else {
            self->_motorControl.setSpeedAndDirection(currentSpeed, motorDirection);
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}