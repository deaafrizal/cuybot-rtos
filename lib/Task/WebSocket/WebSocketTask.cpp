#include <map>
#include <WebSocket/WebSocketTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <freertos/timers.h>

extern int motorSpeed;
extern int motorDirection;
extern int mode;

#define BATTERY_ADC_PIN 0
#define VOLTAGE_DIVIDER_FACTOR 2
#define NO_CLIENT_TIMEOUT_MS 10000
#define PLAYTIME_UPDATE_INTERVAL_MS 1000
#define BATTERY_UPDATE_INTERVAL_MS 5000
#define RESET_TIMEOUT_MS 20000

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;
BatteryMonitorTask WebSocketTask::batteryMonitorTask(BATTERY_ADC_PIN, 3.0, 4.2, VOLTAGE_DIVIDER_FACTOR);

std::map<String, unsigned long> playtimeMap;
std::map<String, bool> isPausedMap;
std::map<String, unsigned long> lastConnectionMap;
std::map<String, int> clientIDMap;

WebSocketTask::WebSocketTask(ModeSelectionTask &modeSelectionTask) : _modeSelectionTask(modeSelectionTask) {
    taskHandle = NULL;
    activeClientCount = 0;
    noClientTimer = NULL;
    isOperationSuspended = false;
}

WebSocketTask::~WebSocketTask() {
    stopTask();
}

void WebSocketTask::startTask(int stackSize) {
    if (taskHandle == NULL) {
        instance = this;
        batteryMonitorTask.startMonitoring();
        xTaskCreate(webSocketTaskFunction, "WebSocketTask", stackSize, this, 4, &taskHandle);
        noClientTimer = xTimerCreate("NoClientTimer", pdMS_TO_TICKS(NO_CLIENT_TIMEOUT_MS), pdTRUE, this, checkForActiveClients);
    }
}

void WebSocketTask::stopTask() {
    if (taskHandle != NULL) {
        vTaskSuspend(taskHandle);
        webSocket.close();
        vTaskDelete(taskHandle);
        batteryMonitorTask.stopMonitoring();
        taskHandle = NULL;

        if (noClientTimer != NULL) {
            xTimerDelete(noClientTimer, 0);
            noClientTimer = NULL;
        }
    }
}

void WebSocketTask::suspendTask() {
    if (!isOperationSuspended && taskHandle != NULL && eTaskGetState(taskHandle) != eSuspended) {
        vTaskSuspend(taskHandle);
        isOperationSuspended = true;
    }
}

void WebSocketTask::resumeTask() {
    if (isOperationSuspended && taskHandle != NULL && eTaskGetState(taskHandle) == eSuspended) {
        vTaskResume(taskHandle);
        isOperationSuspended = false;
    }
}

TaskHandle_t WebSocketTask::getTaskHandle() {
    return taskHandle;
}

void WebSocketTask::webSocketTaskFunction(void *parameter) {
    WebSocketTask *self = static_cast<WebSocketTask*>(parameter);
    self->webSocket.begin();
    self->webSocket.onEvent(onWebSocketEvent);

    unsigned long lastPlaytimeSent = 0;
    unsigned long lastBatterySent = 0;

    for (;;) {
        self->webSocket.loop();
        unsigned long currentMillis = millis();
        
        if (currentMillis - lastPlaytimeSent > PLAYTIME_UPDATE_INTERVAL_MS) {
            self->updatePlaytime();
            self->sendPlaytimeData();
            lastPlaytimeSent = currentMillis;
        }

        if (currentMillis - lastBatterySent > BATTERY_UPDATE_INTERVAL_MS) {
            self->sendBatteryData();
            lastBatterySent = currentMillis;
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    vTaskSuspend(NULL);
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
                        self->_modeSelectionTask.triggerModeChange(newMode);
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

int WebSocketTask::getClientNumFromID(String clientID) {
    if (clientIDMap.find(clientID) != clientIDMap.end()) {
        return clientIDMap[clientID];
    }
    return -1;
}

void WebSocketTask::sendBatteryData() {
    float batteryVoltage = batteryMonitorTask.getBatteryVoltage();
    float batteryPercentage = batteryMonitorTask.getBatteryPercentage();
    
    String jsonData = "{\"batteryVoltage\": " + String(batteryVoltage, 2) + 
                      ", \"batteryPercentage\": " + String(batteryPercentage, 2) + "}";

    if (!activeClients.empty()) {
        webSocket.broadcastTXT(jsonData);
    }
}

void WebSocketTask::sendModeData() {
    String jsonData = "{\"mode\": " + String(mode) + "}";

    if (!activeClients.empty()) {
        webSocket.broadcastTXT(jsonData);
    }
}