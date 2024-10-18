#include <Ultrasonic/UltrasonicTask.h>
#include <Arduino.h>

extern int motorSpeed;
extern int motorDirection;
extern bool userControllingDirection;

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl)
    : _ultrasonic(ultrasonic), _motorControl(motorControl), _distance(0), taskRunning(false), _taskHandle(nullptr) {
    _ultrasonic.begin();  // Initialize the ultrasonic sensor
}

void UltrasonicTask::startTask() {
    if (_taskHandle == nullptr) {
        taskRunning = true;
        xTaskCreate(distanceMeasureTask, "DistanceMeasureTask", 3048, this, 6, &_taskHandle);
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != nullptr) {
        taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("Ultrasonic task stopped.");
    }
}

TaskHandle_t UltrasonicTask::getTaskHandle() {
    return _taskHandle;
}

void UltrasonicTask::suspendTask() {
    if (_taskHandle != nullptr) {
        taskRunning = false;
        vTaskSuspend(_taskHandle);
        Serial.println("Ultrasonic task suspended.");
    }
}

void UltrasonicTask::resumeTask() {
    if (_taskHandle != nullptr) {
        taskRunning = true;
        vTaskResume(_taskHandle);
        Serial.println("Ultrasonic task resumed.");
    }
}

void UltrasonicTask::distanceMeasureTask(void *parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(parameters);

    while (self->taskRunning) {
        self->_distance = self->_ultrasonic.getDistance();

        if (self->_distance > self->_minDistance && self->_distance <= self->_maxDistance) {
            self->_motorControl.stop();
            Serial.println("Obstacle detected! Stopping and turning back.");
            vTaskDelay(pdMS_TO_TICKS(500));

            self->_motorControl.turnRight(200);
            vTaskDelay(pdMS_TO_TICKS(500));

            self->_motorControl.stop();
            vTaskDelay(pdMS_TO_TICKS(500));
        } 

        vTaskDelay(pdMS_TO_TICKS(self->_vdelayTime));
    }
}