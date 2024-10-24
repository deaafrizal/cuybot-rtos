#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <Arduino.h>
#include <WebSocket/WebSocketTask.h>
#include <freertos/semphr.h>

class HardwareMonitorTask {
public:
    HardwareMonitorTask(WebSocketTask* webSocketTask);

    void startTask();
    void suspendTask();
    void resumeTask();
    void stopTask();

    void triggerConnectionCheck();

    TaskHandle_t getTaskHandle();

private:
    const uint32_t _taskStackSize = 2048;
    const UBaseType_t _taskPriority = 3;

    float _freeMemKB;
    float _freeMemMB;
    float _freeMemPercentage;
    bool _isOnline;

    unsigned long _lastSendTime = 0;
    static const unsigned long _DATA_SEND_INTERVAL = 1500;
    static const unsigned long _MONITOR_INTERVAL = 1000;

    TaskHandle_t _taskHandle;
    WebSocketTask* _webSocketTask;

    SemaphoreHandle_t _connectionCheckSemaphore;

    void getFreeMem();
    void checkConnectionStatus();
    void sendHardwareData();
    static void monitorTask(void *pvParameters);
};

#endif


