#ifndef IRTASK_H
#define IRTASK_H

#include <Arduino.h>
#include <IR/ir.h>  // Include your custom IR library

class IRTask {
public:
    IRTask(IR &irSensor);  // Constructor
    void startTask();      // Start the FreeRTOS task to read IR sensors
    void stopTask();       // Stop the FreeRTOS task
    void suspendTask();    // Suspend the FreeRTOS task
    void resumeTask();     // Resume the FreeRTOS task
    TaskHandle_t getTaskHandle();
    bool taskRunning;      // Status of the task (running or not)

private:
    IR &_ir;               // Reference to the IR object
    TaskHandle_t _taskHandle;  // FreeRTOS task handle

    const int _vdelayTime = 100;  // Delay time between readings in milliseconds
    static void irMeasureTask(void *_parameters);  // Static task function for FreeRTOS
};

#endif