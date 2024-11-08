#include <Ultrasonic/UltrasonicTask.h>

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl)
    : _ultrasonic(ultrasonic), _motorControl(motorControl), _initSpeed(60), _taskHandle(NULL), _taskRunning(false) {}

void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(DistanceMeasureTask, "DistanceMeasureTask", _taskStackSize, this, _taskPriority, &_taskHandle);
        Serial.println("Ultrasonic task started.");
    }
}

bool UltrasonicTask::getIsRunning() {
    return _taskRunning;
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        _motorControl.setSpeed(0, 0);
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
                    self->_motorControl.setSpeed(0, 0);
                    stopState = 1;
                } else {
                    self->_motorControl.setSpeed(self->_initSpeed, self->_initSpeed);
                }
                break;

            case 1:
                vTaskDelay(pdMS_TO_TICKS(70));
                stopState = 2;
                break;

            case 2:
            if (distance > 40) {
                self->_motorControl.setSpeed(self->_initSpeed, self->_initSpeed);
                stopState = 0;
            } else {
                self->_motorControl.setSpeed(self->_initSpeed, -self->_initSpeed);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(70));
    }
    Serial.println("Ultrasonic task exiting.");
}