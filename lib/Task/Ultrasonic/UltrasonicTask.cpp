#include <Ultrasonic/UltrasonicTask.h>

extern int motorSpeed;
extern int motorDirection; 

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorTask &motorTask)
    : _ultrasonic(ultrasonic), _taskHandle(NULL), _taskRunning(false), _motorTask(motorTask) {}

void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true; // Set the task running flag to true
        xTaskCreate(DistanceMeasureTask, "DistanceMeasureTask", 3448, this, 4, &_taskHandle);
        Serial.println("Ultrasonic task started.");
    }else {
        Serial.println("Ultrasonic task already running before this call.");
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        motorSpeed = 0;
        motorDirection = 0;
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("Ultrasonic task stopped.");
    }else {
        Serial.println("Ultrasonic task was not running.");
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
                    motorSpeed = 0;
                    motorDirection = 0;
                    stopState = 1;
                } else {
                    motorSpeed = 50;
                    motorDirection = 0;
                }
                break;

            case 1:
                motorSpeed = 0;
                motorDirection = 0;
                vTaskDelay(pdMS_TO_TICKS(70));
                stopState = 2;
                break;

            case 2:
            if (distance > 40) {
                stopState = 0;
            } else {
                motorSpeed = -50;
                motorDirection = -1;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(70));
    }
    Serial.println("Ultrasonic task exiting.");
}