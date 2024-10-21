#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <set>
#include <map>
#include <String>
#include <WebSocketsServer.h>
#include <BatteryMonitor/BatteryMonitorTask.h>
#include <freertos/timers.h>

class ModeSelectionTask;

class WebSocketTask {
public:
    WebSocketTask(ModeSelectionTask &modeSelectionTask);
    ~WebSocketTask();
    
    void startTask(int stackSize = 5295);
    void stopTask();
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

    static void checkForActiveClients(TimerHandle_t xTimer);

    void sendPlaytimeData();
    void sendBatteryData();
    void sendModeData();

private:
    static WebSocketsServer webSocket;
    static WebSocketTask* instance;
    static BatteryMonitorTask batteryMonitorTask;

    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    std::set<String> activeClients;
    std::map<String, unsigned long> playtimeMap;
    std::map<String, bool> isPausedMap;
    std::map<String, unsigned long> lastConnectionMap;
    std::map<String, int> clientIDMap;

    void updatePlaytime();
    int getClientNumFromID(String clientID);

    TaskHandle_t taskHandle;
    bool isOperationSuspended;
    TimerHandle_t noClientTimer;
    int activeClientCount;

    ModeSelectionTask &_modeSelectionTask;
};

#endif