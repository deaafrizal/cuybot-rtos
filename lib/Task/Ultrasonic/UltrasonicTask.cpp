#include <Ultrasonic/UltrasonicTask.h>

extern bool userControllingDirection;

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorTask &motorTask) 
    : _ultrasonic(ultrasonic), _motorTask(motorTask),_distance(0), _taskHandle(NULL), taskRunning(false) {
        ultrasonic.begin();
    }

void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(
            distanceMeasureTask,
            "DistanceMeasureTask",
            3048,
            this,
            6,                     
            &_taskHandle         
        );
        if(_taskHandle != NULL) {
            taskRunning = true; 
            Serial.println("Ultrasonic task started.");
        }
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        taskRunning = false;
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
    int noEchoTime = 0;

    while (self->taskRunning) {
        self->_distance = self->_ultrasonic.getDistance();

        if (self->_distance > self->_minDistance && self->_distance <= self->_maxDistance) {
            Serial.println("Obstacle detected! Turning to avoid.");
            self->_motorTask.setDirection(1);
        } 
        else if (self->_distance > self->_maxDistance && !userControllingDirection){
            self->_motorTask.setDirection(0);
        }

        if (self->_distance == -1) {
            noEchoTime += self->_vdelayTime;
            if (noEchoTime >= self->_timeoutPeriod) {
                Serial.println("No echo detected for 10 seconds. Stopping task...");
                break;
            }
        } else {
            noEchoTime = 0;
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(self->_vdelayTime));
    }
    Serial.println("Ultrasonic task exiting.");
    self->_taskHandle = NULL;
    vTaskDelete(NULL);
}