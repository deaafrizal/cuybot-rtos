#include <Ultrasonic/UltrasonicMonitorTask.h>

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

TaskHandle_t UltrasonicMonitorTask::getTaskHandle() {
    return _taskHandle;
}


void UltrasonicMonitorTask::monitorTask(void *_parameters) {
    UltrasonicMonitorTask *self = static_cast<UltrasonicMonitorTask *>(_parameters);

    int noEchoTime = 0;  // Counter to track how long there has been no echo
    const int maxNoEchoTime = 10000;  // Maximum time (10 seconds) without echo before killing the task

    while (true) {
        // Check if the ultrasonic task is running
        if (!self->_ultrasonicTask.taskRunning) {
            long distance = self->_ultrasonic.getDistance();

            // If an echo is detected, restart the ultrasonic task
            if (distance != -1) {
                Serial.println("Echo detected again. Restarting ultrasonic task...");
                self->_ultrasonicTask.startTask();
                noEchoTime = 0;  // Reset no echo timer when echo is detected
            } else {
                noEchoTime += 5000;  // Accumulate the delay time (5 seconds per iteration)
                Serial.printf("Waiting for ultrasonic sensor to power up... (%d ms of %d ms)\n", noEchoTime, maxNoEchoTime);
            }
        }

        // If no echo for more than 10 seconds, terminate the task
        if (noEchoTime >= maxNoEchoTime) {
            Serial.println("No echo detected for 10 seconds. Terminating monitor task...");
            vTaskDelete(NULL);  // Self-delete the task
        }

        // Delay between checks
        vTaskDelay(pdMS_TO_TICKS(5000));  // 5 second delay between checks
    }
}