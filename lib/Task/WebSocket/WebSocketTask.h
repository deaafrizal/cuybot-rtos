#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <set>
#include <map>
#include <String>
#include <WebSocketsServer.h>
#include <BatteryMonitor/BatteryMonitorTask.h>
#include <StackMonitor/StackMonitorTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>

class WebSocketTask {
public:
    WebSocketTask();
    ~WebSocketTask();
    
    void startTask();
    void stopTask();
    void suspendTask();
    void resumeTask();
    void sendPlaytimeData();
    void sendBatteryData();
    void sendModeData();
    void sendStackData();

    void monitorPlaytime(unsigned long currentMillis);
    void monitorBattery(unsigned long currentMillis);
    void monitorStack(unsigned long currentMillis);

    TaskHandle_t getTaskHandle();

    static void checkForActiveClients(TimerHandle_t xTimer);
    static void setModeSelectionTaskReference(ModeSelectionTask& modeSelectionTask); // Method to set the reference when needed

private:
    const unsigned long stackSize = 4096;
    static WebSocketsServer webSocket;
    static WebSocketTask* instance;
    static BatteryMonitorTask batteryMonitorTask;
    static ModeSelectionTask* modeSelectionTask; // Change to static pointer

    StackMonitorTask* stackMonitorTask;

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
};

#endif