#include <Ultrasonic/UltrasonicTask.h>

extern int motorSpeed;
extern int motorDirection; 

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic)
    : _ultrasonic(ultrasonic), _taskHandle(NULL), _taskRunning(false), speed(0) {}

void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(DistanceMeasureTask, "DistanceMeasureTask", _taskStackSize, this, _taskPriority, &_taskHandle);
        speed = EEPROMConfig::getMemInt(1);
        Serial.println("Ultrasonic task started.");
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        speed = 0;
        motorSpeed = 0;
        motorDirection = 0;
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("Ultrasonic task stopped.");
    }
}

void UltrasonicTask::DistanceMeasureTask(void *parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(parameters);
    static long lastDistance = 0;
    const int threshold = 3;
    static int stopState = 0;


    while (self->_taskRunning) {
        long distance = self->_ultrasonic.getDistance();
        if (abs(distance - lastDistance) < threshold) {
            distance = lastDistance;
        } else {
            lastDistance = distance;
        }

        switch (stopState) {
            case 0:
                if (distance > 0 && distance < 20) {
                    motorSpeed = self->speed;
                    motorDirection = 0;
                    stopState = 1;
                } else {
                    motorSpeed = self->speed;
                    motorDirection = 0;
                }
                break;

            case 1:
                motorSpeed = self->speed;
                motorDirection = 0;
                vTaskDelay(pdMS_TO_TICKS(70));
                stopState = 2;
                break;

            case 2:
            if (distance > 40) {
                stopState = 0;
            } else {
                motorSpeed = -self->speed;
                motorDirection = -1;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(70));
    }
    Serial.println("Ultrasonic task exiting.");
}