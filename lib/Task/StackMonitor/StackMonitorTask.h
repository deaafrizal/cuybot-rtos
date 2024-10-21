#ifndef STACK_MONITOR_TASK_H
#define STACK_MONITOR_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebSocket/WebSocketTask.h>

class StackMonitorTask {
public:
    StackMonitorTask(TaskHandle_t taskHandle, uint32_t stackSize, const char* taskName, WebSocketTask* webSocketTask);
    void startMonitoring();
    void stopMonitoring();
    float getUsedStackPercentage();

private:
    TaskHandle_t _taskHandle;
    uint32_t _stackSize;

    void sendStackData();
    const char* _taskName;
    float _usedStackPercentage = 0.0;
    unsigned long _lastSendTime = 0;
    static const unsigned long SEND_INTERVAL = 5000;

    void readStack();
    WebSocketTask* _webSocketTask;
    static void monitorTask(void *pvParameters);
};

#endif