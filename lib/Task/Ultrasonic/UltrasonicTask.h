#ifndef ULTRASONIC_TASK_H
#define ULTRASONIC_TASK_H

#include <Arduino.h>
#include <Ultrasonic/Ultrasonic.h>

#define ULTRASONIC_FILTER_SIZE 5

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonicSensor);  // Constructor
    void startTask();      // Start the FreeRTOS task to measure distance
    void stopTask();       // Stop the FreeRTOS task
    void suspendTask();    // Suspend the FreeRTOS task
    void resumeTask();     // Resume the FreeRTOS task
    void printLog();       // Print log of distance measured
    bool taskRunning;      // Status of the task (running or not)
    TaskHandle_t getTaskHandle();


private:
    Ultrasonic &_ultrasonic;  // Reference to the Ultrasonic object
    long _distance;           // Holds the current distance measurement

    long readings[ULTRASONIC_FILTER_SIZE];  // Array to hold recent distance readings for filtering
    int index;          // Index for the circular buffer
    long total;                  // Total sum of the readings (for filtering)
    
    TaskHandle_t _taskHandle;    // FreeRTOS task handle

    const int _minDistance = 0;     // Minimum valid distance
    const int _maxDistance = 30;    // Maximum valid distance
    const int _timeoutPeriod = 10000; // Timeout period in milliseconds (10 seconds)
    const int _vdelayTime = 100;    // Delay time between readings in milliseconds
    const int _filter_size = ULTRASONIC_FILTER_SIZE;
    long _smoothedDistance;
    static void distanceMeasureTask(void *_parameters);  // Static task function for FreeRTOS
    long applyExponentialSmoothing(long distance, float alpha);
};

#endif