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
    void sendPlaytimeData();
    void sendBatteryData();
    void sendModeData();

    void monitorPlaytime(unsigned long currentMillis);
    void monitorBattery(unsigned long currentMillis);

    TaskHandle_t getTaskHandle();

    static void checkForActiveClients(TimerHandle_t xTimer);
    static void setModeSelectionTaskReference(ModeSelectionTask& modeSelectionTask); // Method to set the reference when needed
    void sendStackDataToClient(String &jsonData);

private:
    static BatteryMonitorTask batteryMonitorTask;
    static ModeSelectionTask* modeSelectionTask; // Change to static pointer
    static void webSocketTaskFunction(void *parameter);
    static void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

    static WebSocketTask* instance;
    static WebSocketsServer webSocket;
    
    std::set<String> activeClients;
    std::map<String, unsigned long> playtimeMap;
    std::map<String, bool> isPausedMap;
    std::map<String, unsigned long> lastConnectionMap;
    std::map<String, int> clientIDMap;

    void updatePlaytime();

    int getClientNumFromID(String clientID);

    TaskHandle_t _taskHandle;
    bool isOperationSuspended;
    TimerHandle_t noClientTimer;
    int activeClientCount;
};

#endif