#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <WebSocketsServer.h>

class WebSocketTask {
public:
    WebSocketTask();
    ~WebSocketTask();
    
    void startTask(int stackSize = 4096);
    void stopTask();
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    static WebSocketsServer webSocket;
    static WebSocketTask* instance;
    TaskHandle_t taskHandle;
    int activeClientCount;
};

#endif