#include <map>
#include <WebSocket/WebSocketTask.h>
#include <ModeSelection/ModeSelectionTask.h>

#define BATTERY_ADC_PIN 0
#define VOLTAGE_DIVIDER_FACTOR 2
#define NO_CLIENT_TIMEOUT_MS 10000
#define PLAYTIME_UPDATE_INTERVAL_MS 1000
#define BATTERY_UPDATE_INTERVAL_MS 5000

extern int motorSpeed;
extern int motorDirection;
extern int mode;
unsigned long lastPlaytimeSent = 0;
unsigned long lastBatterySent = 0;

std::map<String, unsigned long> playtimeMap;
std::map<String, bool> isPausedMap;
std::map<String, unsigned long> lastConnectionMap;
std::map<String, int> clientIDMap;

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;
BatteryMonitorTask WebSocketTask::batteryMonitorTask(BATTERY_ADC_PIN, 3.0, 4.2, VOLTAGE_DIVIDER_FACTOR);
ModeSelectionTask* WebSocketTask::modeSelectionTask = nullptr;

WebSocketTask::WebSocketTask() {
    _taskHandle = NULL;
    activeClientCount = 0;
    noClientTimer = NULL;
    isOperationSuspended = false;
}

WebSocketTask::~WebSocketTask() {
    stopTask();
}

void WebSocketTask::startTask(TaskHandle_t taskHandle, uint32_t stackSize) {
    if (_taskHandle == NULL) {
        _taskHandle = taskHandle;
        instance = this;
        
        
        batteryMonitorTask.startMonitoring();

        xTaskCreate(webSocketTaskFunction, "WebSocketTask", stackSize, this, 10, &_taskHandle);

        noClientTimer = xTimerCreate("NoClientTimer", pdMS_TO_TICKS(NO_CLIENT_TIMEOUT_MS), pdTRUE, this, checkForActiveClients);
    }
}

void WebSocketTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
        webSocket.close();
        vTaskDelete(_taskHandle);
        batteryMonitorTask.stopMonitoring();
        _taskHandle = NULL;
        if (noClientTimer != NULL) {
            xTimerDelete(noClientTimer, 0);
            noClientTimer = NULL;
        }
    }
}

void WebSocketTask::suspendTask() {
    if (!isOperationSuspended && _taskHandle != NULL && eTaskGetState(_taskHandle) != eSuspended) {
        vTaskSuspend(_taskHandle);
        isOperationSuspended = true;
    }
}

void WebSocketTask::resumeTask() {
    if (isOperationSuspended && _taskHandle != NULL && eTaskGetState(_taskHandle) == eSuspended) {
        vTaskResume(_taskHandle);
        isOperationSuspended = false;
    }
}

TaskHandle_t WebSocketTask::getTaskHandle() {
    return _taskHandle;
}

void WebSocketTask::webSocketTaskFunction(void *parameter) {
    WebSocketTask *self = static_cast<WebSocketTask*>(parameter);
    self->webSocket.begin();
    self->webSocket.onEvent(onWebSocketEvent);

    for (;;) {
        self->webSocket.loop();
        unsigned long currentMillis = millis();
        self->monitorPlaytime(currentMillis);
        self->monitorBattery(currentMillis);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    vTaskSuspend(NULL);
}

void WebSocketTask::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    WebSocketTask *self = WebSocketTask::instance;
    String clientID = self->webSocket.remoteIP(num).toString();
    switch (type) {
        case WStype_DISCONNECTED: {
            if (self->activeClientCount > 0) {
                self->activeClientCount--;
            }
            self->isPausedMap[clientID] = true;
            auto it = self->activeClients.find(clientID);
            if (it != self->activeClients.end()) {
                self->activeClients.erase(it);
            }
            self->clientIDMap.erase(clientID);
            if (self->activeClients.empty() && self->noClientTimer != NULL) {
                xTimerStart(self->noClientTimer, 0);
            }
            break;
        }
        case WStype_CONNECTED: {
            self->activeClientCount++;
            self->activeClients.insert(clientID);
            self->clientIDMap[clientID] = num;
            self->sendModeData();
            self->isPausedMap[clientID] = false;
            if (self->isOperationSuspended) {
                self->resumeTask();
            }
            if (self->noClientTimer != NULL) {
                xTimerStop(self->noClientTimer, 0);
            }
            break;
        }
        case WStype_TEXT: {
            if (self->activeClientCount > 0) {
                String receivedData = String((char*)payload);
                int sIndex = receivedData.indexOf('S');
                int dIndex = receivedData.indexOf('D');
                int mIndex = receivedData.indexOf('M');
                if (sIndex != -1 && dIndex != -1 && dIndex > sIndex + 1) {
                    int newSpeed = receivedData.substring(sIndex + 1, dIndex).toInt();
                    int newDirection = receivedData.substring(dIndex + 1).toInt();
                    motorSpeed = newSpeed;
                    motorDirection = newDirection;
                } else if (mIndex != -1 && mIndex < receivedData.length() - 1) {
                    int newMode = receivedData.substring(mIndex + 1).toInt();
                    if (newMode > 0) {
                        modeSelectionTask->triggerModeChange(newMode);
                        self->sendModeData();
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

int WebSocketTask::getClientNumFromID(String clientID) {
    if (clientIDMap.find(clientID) != clientIDMap.end()) {
        return clientIDMap[clientID];
    }
    return -1;
}

void WebSocketTask::monitorPlaytime(unsigned long currentMillis) {
    if (currentMillis - lastPlaytimeSent > PLAYTIME_UPDATE_INTERVAL_MS) {
        updatePlaytime();
        sendPlaytimeData();
        lastPlaytimeSent = currentMillis;
    }
}

void WebSocketTask::monitorBattery(unsigned long currentMillis) {
    if (currentMillis - lastBatterySent > BATTERY_UPDATE_INTERVAL_MS) {
        sendBatteryData();
        lastBatterySent = currentMillis;
    }
}

void WebSocketTask::checkForActiveClients(TimerHandle_t xTimer) {
    WebSocketTask *self = static_cast<WebSocketTask*>(pvTimerGetTimerID(xTimer));
    if (self->activeClients.empty()) {
        if (!self->isOperationSuspended) {
            self->suspendTask();
        }
    }
}

void WebSocketTask::updatePlaytime() {
    unsigned long currentMillis = millis();
    for (const auto& client : activeClients) {
        String clientID = client;
        if (!isPausedMap[clientID]) {
            playtimeMap[clientID] += currentMillis - lastConnectionMap[clientID];
        }
        lastConnectionMap[clientID] = currentMillis;
    }
}

void WebSocketTask::sendPlaytimeData() {
    for (const auto& client : activeClients) {
        String clientID = client;
        unsigned long playtimeSeconds = playtimeMap[clientID] / 1000;
        String jsonData = "{\"playtime\": " + String(playtimeSeconds) + "}";
        int clientNum = getClientNumFromID(clientID);
        if (clientNum >= 0) {
            webSocket.sendTXT(clientNum, jsonData);
        }
    }
}

void WebSocketTask::sendBatteryData() {
    float batteryVoltage = batteryMonitorTask.getBatteryVoltage();
    float batteryPercentage = batteryMonitorTask.getBatteryPercentage();
    String jsonData = "{\"batteryVoltage\": " + String(batteryVoltage, 2) + 
                      ", \"batteryPercentage\": " + String(batteryPercentage, 2) + "}";
    for (const auto& client : activeClients) {
        int clientNum = getClientNumFromID(client);
        if (clientNum >= 0) {
            webSocket.sendTXT(clientNum, jsonData);
        }
    }
}

void WebSocketTask::sendModeData() {
    String jsonData = "{\"mode\": " + String(mode) + "}";
    if (!activeClients.empty()) {
        webSocket.broadcastTXT(jsonData);
    }
}

void WebSocketTask::sendCurrentSpeedAndDirectionData() {
    if (mode != 1 && motorSpeed != 0 && motorDirection != 0) {
        for (const auto& client : activeClients) {
            String clientID = client;
            int currentSpeed = motorSpeed;
            int currentDirection = motorDirection;
            String jsonData = "{\"currentSpeed\": " + String(currentSpeed) + 
                        ", \"currentDirection\": " + String(currentDirection) + "}";
            int clientNum = getClientNumFromID(clientID);
            if (clientNum >= 0) {
             webSocket.sendTXT(clientNum, jsonData);
            }
        }
    }
}

void WebSocketTask::sendStackDataToClient(String &jsonData) {
    for (const auto& client : activeClients) {
        int clientNum = getClientNumFromID(client);
        if (clientNum >= 0) {
            webSocket.sendTXT(clientNum, jsonData);
        }
    }
}
