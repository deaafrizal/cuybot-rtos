#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <set>
#include <map>
#include <String>
#include <WebSocketsServer.h>
#include <BatteryMonitor/BatteryMonitorTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

class WebSocketTask {
public:
    WebSocketTask();
    ~WebSocketTask();
    
    void startTask(TaskHandle_t taskHandle, uint32_t stackSize);
    void stopTask();
    void suspendTask();
    void resumeTask();
    void sendStackDataToClient(String &jsonData);

    TaskHandle_t getTaskHandle();

    static void checkForActiveClients(TimerHandle_t xTimer);
    static void setModeSelectionTaskReference(ModeSelectionTask& modeSelectionTask);

private:
    static BatteryMonitorTask batteryMonitorTask;
    static ModeSelectionTask* modeSelectionTask; // Change to static pointer
    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    static WebSocketTask* instance;
    static WebSocketsServer webSocket;

    bool isOperationSuspended;
    int activeClientCount;
    int getClientNumFromID(String clientID);
    void sendPlaytimeData();
    void updatePlaytime();
    void sendCurrentSpeedAndDirectionData();
    void sendBatteryData();
    void sendModeData();

    void monitorPlaytime(unsigned long currentMillis);
    void monitorBattery(unsigned long currentMillis);
    
    std::set<String> activeClients;
    std::map<String, unsigned long> playtimeMap;
    std::map<String, bool> isPausedMap;
    std::map<String, unsigned long> lastConnectionMap;
    std::map<String, int> clientIDMap;

    TaskHandle_t _taskHandle;
    TimerHandle_t noClientTimer;
};

#endif