#include "UltrasonicMonitorTask.h"

// Constructor: Initialize with a reference to the Ultrasonic sensor and the Ultrasonic task
UltrasonicMonitorTask::UltrasonicMonitorTask(Ultrasonic &ultrasonicSensor, UltrasonicTask &ultrasonicTask)
    : _ultrasonic(ultrasonicSensor), _ultrasonicTask(ultrasonicTask), _taskHandle(NULL) {}

// Start the FreeRTOS task for monitoring the ultrasonic sensor
void UltrasonicMonitorTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(
            monitorTask,           // Task function
            "MonitorUltrasonic",   // Task name
            2048,                  // Stack size
            this,                  // Parameter passed to task
            1,                     // Task priority
            &_taskHandle           // Task handle for controlling the task
        );
    }
}

// Stop the monitor task by deleting it
void UltrasonicMonitorTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);  // Delete the monitor task
        _taskHandle = NULL;        // Clear the task handle
    }
}

// Static function for monitoring the ultrasonic sensor
void UltrasonicMonitorTask::monitorTask(void *_parameters) {
    UltrasonicMonitorTask *self = static_cast<UltrasonicMonitorTask *>(_parameters);

    while (true) {
        // Check if the ultrasonic task is running
        if (!self->_ultrasonicTask.taskRunning) {
            long distance = self->_ultrasonic.getDistance();

            // If an echo is detected, restart the ultrasonic task
            if (distance != -1) {
                Serial.println("Echo detected again. Restarting ultrasonic task...");
                self->_ultrasonicTask.startTask();
            } else {
                Serial.println("Waiting for ultrasonic sensor to power up...");
            }
        }

        // Delay between checks
        vTaskDelay(pdMS_TO_TICKS(5000));  // 1 second delay
    }
}