#include <WebSocket/WebSocketTask.h>
#include <ModeSelection/ModeSelectionTask.h>

extern int motorSpeed;
extern int motorDirection;
extern int mode;

#define BATTERY_ADC_PIN 0
#define VOLTAGE_DIVIDER_FACTOR 2

WebSocketsServer WebSocketTask::webSocket = WebSocketsServer(81);
WebSocketTask* WebSocketTask::instance = nullptr;
BatteryMonitorTask WebSocketTask::batteryMonitorTask(BATTERY_ADC_PIN, 3.0, 4.2, VOLTAGE_DIVIDER_FACTOR);

WebSocketTask::WebSocketTask(ModeSelectionTask &modeSelectionTask): _modeSelectionTask(modeSelectionTask) {
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
        batteryMonitorTask.startMonitoring();
        xTaskCreate(webSocketTaskFunction, "WebSocketTask", stackSize, this, 4, &taskHandle);
        Serial.println("WebSocket task created and running.");
    }
}

void WebSocketTask::stopTask() {
    Serial.println("WS TASK: Stopping...");
    if (taskHandle != NULL) {
        vTaskSuspend(taskHandle);
        webSocket.close();
        vTaskDelete(taskHandle);
        batteryMonitorTask.stopMonitoring();
        taskHandle = NULL;
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

TaskHandle_t WebSocketTask::getTaskHandle() {
    return taskHandle;
}

void WebSocketTask::webSocketTaskFunction(void *parameter) {
    Serial.println("WS Task: beginning event...");
    WebSocketTask *self = static_cast<WebSocketTask*>(parameter);
    self->webSocket.begin();
    self->webSocket.onEvent(onWebSocketEvent);

    for (;;) {
        self->webSocket.loop();
        
        static unsigned long lastSent = 0;
        if (millis() - lastSent > 5000) {
            self->sendBatteryData();
            lastSent = millis();
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    vTaskSuspend(NULL);
}

void WebSocketTask::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    WebSocketTask *self = WebSocketTask::instance;

    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            if (self->activeClientCount > 0) {
                self->activeClientCount--;
                Serial.printf("Active clients: %d\n", self->activeClientCount);
                self->suspendTask();
            }
            break;

        case WStype_CONNECTED: {
            self->activeClientCount++;
            self->sendModeData();
            Serial.print("Client connected: ");
            Serial.println(self->activeClientCount);
            break;
        }

        case WStype_TEXT:
            if(self->activeClientCount > 0) {
                String receivedData = String((char*)payload);

                int sIndex = receivedData.indexOf('S'); //speed
                int dIndex = receivedData.indexOf('D'); //direction
                int mIndex = receivedData.indexOf('M'); //mode
                
                // Check for motor control commands
                if (sIndex != -1 && dIndex != -1 && dIndex > sIndex + 1) {
                    int newSpeed = receivedData.substring(sIndex + 1, dIndex).toInt();
                    int newDirection = receivedData.substring(dIndex + 1).toInt();

                    motorSpeed = newSpeed;
                    motorDirection = newDirection;
                } 
                else if (mIndex != -1 && mIndex < receivedData.length() - 1) {
                    int newMode = receivedData.substring(mIndex + 1).toInt();
                    if(newMode > 0) {
                        self->_modeSelectionTask.triggerModeChange(newMode);
                        self->sendModeData();
                    }
                }
            }
            break;

        default:
            break;
    }
}

void WebSocketTask::sendBatteryData() {
    float batteryVoltage = batteryMonitorTask.getBatteryVoltage();
    float batteryPercentage = batteryMonitorTask.getBatteryPercentage();
    
    String jsonData = "{\"batteryVoltage\": " + String(batteryVoltage, 2) + 
                        ", \"batteryPercentage\": " + String(batteryPercentage, 2) + "}";

    if (activeClientCount > 0) {
        webSocket.broadcastTXT(jsonData);
    }
}

void WebSocketTask::sendModeData() {
    String jsonData = "{\"mode\": " + String(mode) + "}";
    
    if (activeClientCount > 0) {
        webSocket.broadcastTXT(jsonData);
    }
}