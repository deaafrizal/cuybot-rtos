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
            2048,
            this,
            1,                     
            &_taskHandle         
        );
        taskRunning = true; 
    }
}

// Stop the FreeRTOS task
void UltrasonicTask::stopTask() {
    if (_taskHandle != NULL) {
        _taskHandle = NULL;  
        taskRunning = false;
        Serial.println("Task stopped, task handle cleared.");
    }
}

// Suspend the FreeRTOS task
void UltrasonicTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle); 
    }
}

void UltrasonicTask::resumeTask() {
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle); 
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
    // If the first reading, initialize smoothed distance with the first distance
    if (_smoothedDistance == 0) {
        _smoothedDistance = newDistance;
    } else {
        // Apply exponential smoothing formula
        _smoothedDistance = alpha * newDistance + (1 - alpha) * _smoothedDistance;
    }
    return _smoothedDistance;
}

void UltrasonicTask::distanceMeasureTask(void *_parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(_parameters);

    int noEchoTime = 0;
    float alpha = 0.2;  // Smoothing factor (you can adjust this value for more/less smoothing)

    while (true) {
        // Get the raw distance value from the sensor
        self->_distance = self->_ultrasonic.getDistance();

        // Apply exponential smoothing to the distance
        long smoothedValue = self->applyExponentialSmoothing(self->_distance, alpha);

        // If no echo is detected (-1), increment noEchoTime
        if (self->_distance == -1) {
            noEchoTime += self->_vdelayTime;
        } else {
            noEchoTime = 0;
        }

        // Print the smoothed distance value
        self->printLog();

        // Check if no valid echo has been detected for the timeout period
        if (noEchoTime >= self->_timeoutPeriod) {
            Serial.println("No echo detected for 10 seconds. Stopping task...");
            self->stopTask();
            break;
        }

        // Wait for the next measurement
        vTaskDelay(pdMS_TO_TICKS(self->_vdelayTime));
    }

    Serial.println("Task deleted, FreeRTOS cleanup.");
    vTaskDelete(NULL);
}