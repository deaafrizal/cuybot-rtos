#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <set>
#include <map>
#include <String>
#include <WebSocketsServer.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <freertos/semphr.h>
#include <freertos/timers.h>

class WebSocketTask {
public:
    WebSocketTask();
    ~WebSocketTask();
    
    void startTask();
    void stopTask();
    void suspendTask();
    void resumeTask();
    void sendDataToClient(String &jsonData);

    TaskHandle_t getTaskHandle();

    static void checkForActiveClients(TimerHandle_t xTimer);
    static void setModeSelectionTaskReference(ModeSelectionTask& modeSelectionTask);

private:
    const uint32_t _taskStackSize = 8192;
    const UBaseType_t _taskPriority = 10;
    bool isOperationSuspended;

    static ModeSelectionTask* modeSelectionTask;
    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    static WebSocketTask* instance;
    static WebSocketsServer webSocket;

    int activeClientCount;
    int getClientNumFromID(String clientID);
    void sendPlaytimeData();
    void updatePlaytime();
    void sendModeData();

    void monitorPlaytime(unsigned long currentMillis);
    
    std::set<String> activeClients;
    std::map<String, unsigned long> playtimeMap;
    std::map<String, bool> isPausedMap;
    std::map<String, unsigned long> lastConnectionMap;
    std::map<String, int> clientIDMap;

    TaskHandle_t _taskHandle;
    TimerHandle_t noClientTimer;
};

#endif