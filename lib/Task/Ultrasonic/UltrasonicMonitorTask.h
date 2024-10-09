#ifndef ULTRASONIC_MONITOR_TASK_H
#define ULTRASONIC_MONITOR_TASK_H

#include <Arduino.h>
#include <Ultrasonic/UltrasonicTask.h>  // Include the Ultrasonic task

class UltrasonicMonitorTask {
public:
    UltrasonicMonitorTask(Ultrasonic &ultrasonicSensor, UltrasonicTask &ultrasonicTask);  // Constructor
    void startTask();    // Start the FreeRTOS task for monitoring
    void stopTask();     // Stop the monitor task
    TaskHandle_t getTaskHandle();

private:
    Ultrasonic &_ultrasonic;         // Reference to the Ultrasonic object
    UltrasonicTask &_ultrasonicTask; // Reference to the Ultrasonic task
    TaskHandle_t _taskHandle;        // FreeRTOS task handle

    static void monitorTask(void *_parameters);  // Static task function for FreeRTOS
};

#endif