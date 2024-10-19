#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <Arduino.h>

class HardwareMonitor {
public:
    HardwareMonitor();

    void startTask();
    void suspendTask();
    void resumeTask();
    void stopTask();

    TaskHandle_t getTaskHandle();

private:
    TaskHandle_t _taskHandle;
    static void monitorTask(void *parameter);
};

#endif