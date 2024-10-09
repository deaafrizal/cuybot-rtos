#include <Ultrasonic/UltrasonicTask.h>

// Constructor: Initialize with a reference to the Ultrasonic sensor
UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonicSensor) 
    : _ultrasonic(ultrasonicSensor), _distance(0), _smoothedDistance(0), _taskHandle(NULL), taskRunning(false) {}

// Start the FreeRTOS task to measure distance
void UltrasonicTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(
            distanceMeasureTask,
            "DistanceMeasureTask",
            4096,
            this,
            5,                     
            &_taskHandle         
        );
        if(_taskHandle != NULL) {
            taskRunning = true; 
            Serial.println("Ultrasonic task started.");
        }
    }
}

// Stop the FreeRTOS task
void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        taskRunning = false;  // Set flag to stop the task
        Serial.println("Ultrasonic task stop requested.");
    }
}

TaskHandle_t UltrasonicTask::getTaskHandle() {
    return _taskHandle;
}

// Suspend the FreeRTOS task
void UltrasonicTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
        Serial.println("Ultrasonic task suspended.");
    }
}

// Resume the FreeRTOS task
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

// Apply exponential smoothing to the distance values
long UltrasonicTask::applyExponentialSmoothing(long newDistance, float alpha) {
    if (_smoothedDistance == 0) {
        _smoothedDistance = newDistance;  // Initialize smoothed distance
    } else {
        _smoothedDistance = alpha * newDistance + (1 - alpha) * _smoothedDistance;  // Apply smoothing
    }
    return _smoothedDistance;
}

void UltrasonicTask::distanceMeasureTask(void *_parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(_parameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int noEchoTime = 0;  // Tracks time without a valid echo
    float alpha = 0.2;   // Smoothing factor

    while (self->taskRunning) {
        // Get the raw distance value from the sensor
        self->_distance = self->_ultrasonic.getDistance();

        // Apply exponential smoothing to the distance
        self->applyExponentialSmoothing(self->_distance, alpha);

        // Print the smoothed distance
        self->printLog();

        // Check for no echo (-1)
        if (self->_distance == -1) {
            noEchoTime += self->_vdelayTime;
            if (noEchoTime >= self->_timeoutPeriod) {
                Serial.println("No echo detected for 10 seconds. Stopping task...");
                break;
            }
        } else {
            noEchoTime = 0;  // Reset no-echo counter
        }

        // Wait for the next measurement
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(self->_vdelayTime));
    }

    Serial.println("Ultrasonic task exiting.");
    self->_taskHandle = NULL;  // Clear task handle after exiting
    vTaskDelete(NULL);  // Delete the task
}