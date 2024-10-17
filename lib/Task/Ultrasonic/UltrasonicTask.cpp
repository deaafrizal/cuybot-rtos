#include <Ultrasonic/UltrasonicTask.h>

extern bool userControllingDirection;
extern int motorSpeed;

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorTask &motorTask) 
    : _ultrasonic(ultrasonic), _motorTask(motorTask),_distance(0), _taskHandle(NULL), taskRunning(false) {
        ultrasonic.begin();
    }

void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        taskRunning = true;
        xTaskCreate(
            distanceMeasureTask,
            "DistanceMeasureTask",
            3048,
            this,
            6,                     
            &_taskHandle         
        );
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("Ultrasonic task stop requested.");
    }
}

TaskHandle_t UltrasonicTask::getTaskHandle() {
    return _taskHandle;
}

void UltrasonicTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
        Serial.println("Ultrasonic task suspended.");
    }
}

void UltrasonicTask::resumeTask() {
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle); 
        Serial.println("Ultrasonic task resumed.");
    }
}

void UltrasonicTask::distanceMeasureTask(void *_parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(_parameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (self->taskRunning) {
        self->_distance = self->_ultrasonic.getDistance();

        if (self->_distance > self->_minDistance && self->_distance <= self->_maxDistance) {
            Serial.println("Obstacle detected! Turning to avoid.");
            self->_motorTask.setDirection(1);
            self->_motorTask.setSpeed(motorSpeed * 0.5);
        } 
        else if (self->_distance > self->_maxDistance && !userControllingDirection) {
            self->_motorTask.setDirection(0);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(self->_vdelayTime));
    }
}