#include "StackMonitorTask.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

StackMonitorTask::StackMonitorTask(TaskHandle_t taskHandle, uint32_t stackSize, const char* taskName, WebSocketTask* webSocketTask )
    : _taskHandle(taskHandle), _stackSize(stackSize), _taskName(taskName), _webSocketTask(webSocketTask) {}

void StackMonitorTask::startMonitoring() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "StackMonitorTask", 2048, this, 1, &_taskHandle);
    }
}

void StackMonitorTask::stopMonitoring() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

void StackMonitorTask::monitorTask(void *pvParameters) {
    StackMonitorTask *self = static_cast<StackMonitorTask*>(pvParameters);
    while (true) {
        self->readStack();
        self->sendStackData();
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void StackMonitorTask::readStack() {
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(_taskHandle);
    unsigned long currentFreeStackSpace = highWaterMark * sizeof(StackType_t);
    unsigned long usedStackSpace = _stackSize - currentFreeStackSpace;
    _usedStackPercentage = (float)usedStackSpace / _stackSize * 100.0;
}

float StackMonitorTask::getUsedStackPercentage() {
    return _usedStackPercentage;
}

void StackMonitorTask::sendStackData() {
    unsigned long currentMillis = millis();
    
    if (currentMillis - _lastSendTime >= SEND_INTERVAL) {
        float usedStackPercentage = getUsedStackPercentage();
       String jsonData = "{\"usedStackPercentage\": " + String(usedStackPercentage) + 
                          ", \"taskName\": \"" + String(_taskName) + "\"}"; 
                          
        _webSocketTask->sendStackDataToClient(jsonData);
        _lastSendTime = currentMillis;
    }
}