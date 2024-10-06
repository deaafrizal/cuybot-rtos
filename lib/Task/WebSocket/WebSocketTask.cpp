#include <WebSocket/WebSocketTask.h>

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;

WebSocketTask::WebSocketTask() {
    taskHandle = NULL;
    activeClientCount = 0;
}

WebSocketTask::~WebSocketTask() {
    stopTask();
}

void WebSocketTask::startTask(int stackSize) {
    if (taskHandle == NULL) {
        instance = this;
        xTaskCreate(webSocketTaskFunction, "WebSocketTask", stackSize, this, 1, &taskHandle);
        Serial.println("WebSocket task created and running.");
    }
}

void WebSocketTask::stopTask() {
    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
        webSocket.close();
        Serial.println("WebSocket task stopped and deleted.");
    }
}

void WebSocketTask::suspendTask() {
    if (taskHandle != NULL && eTaskGetState(taskHandle) != eSuspended) {
        vTaskSuspend(taskHandle);
        Serial.println("WebSocket task suspended.");
    }
}

void WebSocketTask::resumeTask() {
    if (taskHandle != NULL && eTaskGetState(taskHandle) == eSuspended) {
        vTaskResume(taskHandle);
        Serial.println("WebSocket task resumed.");
    }
}

// Return the task handle
TaskHandle_t WebSocketTask::getTaskHandle() {
    return taskHandle;
}

// WebSocket task function
void WebSocketTask::webSocketTaskFunction(void *parameter) {
    WebSocketTask *self = static_cast<WebSocketTask*>(parameter);
    self->webSocket.begin();
    self->webSocket.onEvent(onWebSocketEvent);

    for (;;) {
        self->webSocket.loop();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void WebSocketTask::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    WebSocketTask *self = WebSocketTask::instance;

    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            if (self->activeClientCount > 0) {
                self->activeClientCount--;
                Serial.printf("Active clients: %d\n", self->activeClientCount);
            }
            break;

        case WStype_CONNECTED: {
            self->activeClientCount++;
            Serial.print("client connected: ");
            Serial.println(self->activeClientCount);
            break;
        }

        case WStype_TEXT:
            if(self->activeClientCount > 0) {
                Serial.printf("[%u] Received text: %s\n", num, payload);
            }
            // Handle your WebSocket messages here (e.g., motor control commands)
            break;

        default:
            break;
    }
}