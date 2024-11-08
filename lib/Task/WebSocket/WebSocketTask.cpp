#include <WebSocket/WebSocketTask.h>
#include <freertos/semphr.h>

#define NO_CLIENT_TIMEOUT_MS 10000
#define PLAYTIME_UPDATE_INTERVAL_MS 1000

extern int motorSpeed;
extern int motorDirection;
extern int mode;
unsigned long lastPlaytimeSent = 0;

std::map<String, unsigned long> playtimeMap;
std::map<String, bool> isPausedMap;
std::map<String, unsigned long> lastConnectionMap;
std::map<String, int> clientIDMap;

SemaphoreHandle_t xSemaphore;

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;
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

void WebSocketTask::startTask() {
    if (_taskHandle == NULL) {
        instance = this;
        
        xSemaphore = xSemaphoreCreateMutex();

        xTaskCreate(webSocketTaskFunction, "WebSocketTask", _taskStackSize, this, _taskPriority, &_taskHandle);

        noClientTimer = xTimerCreate("NoClientTimer", pdMS_TO_TICKS(NO_CLIENT_TIMEOUT_MS), pdTRUE, this, checkForActiveClients);
    }
}

void WebSocketTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
        webSocket.close();
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        if (noClientTimer != NULL) {
            xTimerDelete(noClientTimer, 0);
            noClientTimer = NULL;
        }
        if (xSemaphore != NULL) {
            vSemaphoreDelete(xSemaphore);
            xSemaphore = NULL;
        }
    }
    vTaskSuspend(NULL);
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
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
            self->monitorPlaytime(currentMillis);
            xSemaphoreGive(xSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void WebSocketTask::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    WebSocketTask *self = WebSocketTask::instance;
    String clientID = self->webSocket.remoteIP(num).toString();
    
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
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
                if (self->activeClientCount > 0 && payload != nullptr) {
                    char* receivedData = (char*)payload;

                    char* sIndex = strchr(receivedData, 'S');
                    char* dIndex = strchr(receivedData, 'D');
                    char* mIndex = strchr(receivedData, 'M');

                    if (sIndex != nullptr && dIndex != nullptr && dIndex > sIndex + 1) {
                        int newSpeed = atoi(sIndex + 1);
                        int newDirection = atoi(dIndex + 1);
                        motorSpeed = newSpeed;
                        motorDirection = newDirection;
                    }
                    else if (mIndex != nullptr && mIndex < (receivedData + strlen(receivedData) - 1)) {
                        int newMode = atoi(mIndex + 1);
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
        xSemaphoreGive(xSemaphore);
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

void WebSocketTask::sendModeData() {
    String jsonData = "{\"mode\": " + String(mode) + "}";
    if (!activeClients.empty()) {
        webSocket.broadcastTXT(jsonData);
    }
}

void WebSocketTask::sendDataToClient(String &jsonData) {
    for (const auto& client : activeClients) {
        int clientNum = getClientNumFromID(client);
        if (clientNum >= 0) {
            webSocket.sendTXT(clientNum, jsonData);
        }
    }
}