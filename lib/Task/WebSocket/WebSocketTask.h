#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <WebSocketsServer.h>
#include <BatteryMonitor/BatteryMonitorTask.h>

class WebSocketTask {
public:
    WebSocketTask();
    ~WebSocketTask();
    
    void startTask(int stackSize = 6000);
    void stopTask();
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    static BatteryMonitorTask batteryMonitorTask;
    static WebSocketsServer webSocket;
    static WebSocketTask* instance;

    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
    
    TaskHandle_t taskHandle;
    int activeClientCount;

    void sendBatteryData();
};

#endif