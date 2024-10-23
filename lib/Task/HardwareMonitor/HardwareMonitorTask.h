#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <Arduino.h>
#include <WebSocket/WebSocketTask.h>

class HardwareMonitorTask {
public:
    HardwareMonitorTask(WebSocketTask* webSocketTask);

    void startTask();
    void suspendTask();
    void resumeTask();
    void stopTask();

    TaskHandle_t getTaskHandle();

private:
    const int _taskStackSize = 2048;
    const int _taskPriority = 1;
    float _freeMemKB;
    float _freeMemMB;
    float _freeMemPercentage;
    unsigned long _lastSendTime = 0;
    static const unsigned long _DATA_SEND_INTERVAL = 1500;  // Send interval is now decoupled from monitoring
    static const unsigned long _MONITOR_INTERVAL = 1000;    // Memory monitoring interval

    TaskHandle_t _taskHandle;
    WebSocketTask* _webSocketTask;

    void getFreeMem();               // Collects memory data more frequently
    void sendFreeMemData();          // Sends the most recent memory data
    static void monitorTask(void *pvParameters);
};

#endif