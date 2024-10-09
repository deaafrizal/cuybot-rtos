#include <WebSocket/WebSocketTask.h>

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;

WebSocketTask::WebSocketTask() {
    Serial.println("WS Task initialize...");

    taskHandle = NULL;
    activeClientCount = 0;
}

WebSocketTask::~WebSocketTask() {
    Serial.println("WS Task cleanup...");
    stopTask();
}

void WebSocketTask::startTask(int stackSize) {
    Serial.println("WS Task: starting up...");

    if (taskHandle == NULL) {
        instance = this;
        xTaskCreate(webSocketTaskFunction, "WebSocketTask", stackSize, this, 6, &taskHandle);
        Serial.println("WebSocket task created and running.");
    }
}

void WebSocketTask::stopTask() {
    Serial.println("WS TASK: Stoping...");

    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
        webSocket.close();
        Serial.println("WebSocket task stopped and deleted.");
    }
}

void WebSocketTask::suspendTask() {
    Serial.println("WS Task: suspending...");
    if (taskHandle != NULL && eTaskGetState(taskHandle) != eSuspended) {
        vTaskSuspend(taskHandle);
        Serial.println("WebSocket task suspended.");
    }
}

void WebSocketTask::resumeTask() {
    Serial.println("WS Task: resuming...");
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
    Serial.println("WS Task: begining event...");
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