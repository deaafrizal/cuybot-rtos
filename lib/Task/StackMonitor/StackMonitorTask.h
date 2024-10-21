#ifndef STACK_MONITOR_TASK_H
#define STACK_MONITOR_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class StackMonitorTask {
public:
    StackMonitorTask(TaskHandle_t getTaskHandle, uint32_t stackSize);
    
    void startMonitoring();          // Start the FreeRTOS task for monitoring stack
    void stopMonitoring();           // Stop the FreeRTOS task
    float getUsedStackPercentage();  // Get the last read used stack percentage

private:
    TaskHandle_t _taskHandle;       // Handle for the task being monitored
    TaskHandle_t _getTaskHandle;       // Handle for the task being monitored
    uint32_t _stackSize;             // Size of the stack to monitor
    float _usedStackPercentage;

    static void monitorTask(void *pvParameters);  // FreeRTOS task to monitor stack usage
    void readStack();                // Function to read and calculate stack usage
};

#endif