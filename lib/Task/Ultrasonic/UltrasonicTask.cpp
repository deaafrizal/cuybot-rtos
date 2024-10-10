#include <Ultrasonic/UltrasonicTask.h>

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorTask &motorTask) 
    : _ultrasonic(ultrasonic), _motorTask(motorTask),_distance(0), _smoothedDistance(0), _taskHandle(NULL), taskRunning(false) {
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

void UltrasonicTask::printLog() {
    if (_smoothedDistance == -1) {
        Serial.println("Error: No echo detected");
    } else {
        Serial.print("Smoothed Distance: ");
        Serial.print(_smoothedDistance);
        Serial.println(" cm");
    }
}

long UltrasonicTask::applyExponentialSmoothing(long newDistance, float alpha) {
    if (_smoothedDistance == 0) {
        _smoothedDistance = newDistance;
    } else {
        _smoothedDistance = alpha * newDistance + (1 - alpha) * _smoothedDistance;
    }
    return _smoothedDistance;
}

void UltrasonicTask::distanceMeasureTask(void *_parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(_parameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int noEchoTime = 0;
    float alpha = 0.2;

    while (self->taskRunning) {
        self->_distance = self->_ultrasonic.getDistance();

        long range = self->applyExponentialSmoothing(self->_distance, alpha);

        if (range != -1 && range < 12) {
            self->_motorTask.setSpeedAndDirection(0, 0);
        }

        if (range == -1) {
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