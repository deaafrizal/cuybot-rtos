#ifndef STACK_MONITOR_TASK_H
#define STACK_MONITOR_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class StackMonitorTask {
public:
    StackMonitorTask(TaskHandle_t taskHandle, uint32_t stackSize, const char* taskName);
    void startMonitoring();
    void stopMonitoring();
    float getUsedStackPercentage();

private:
    TaskHandle_t _taskHandle;
    uint32_t _stackSize;
    
    const char* _taskName;
    float _usedStackPercentage = 0.0;

    void readStack();
    
    static void monitorTask(void *pvParameters);
};

#endif