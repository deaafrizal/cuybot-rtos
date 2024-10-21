#include "StackMonitorTask.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

StackMonitorTask::StackMonitorTask(TaskHandle_t getTaskHandle, uint32_t stackSize)
    : _getTaskHandle(getTaskHandle), _stackSize(stackSize), _taskHandle(NULL), _usedStackPercentage(0.0){}

void StackMonitorTask::startMonitoring() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "StackMonitorTask", 2048, this, 1, &_taskHandle);
    }
}

void StackMonitorTask::stopMonitoring() {
    if (_taskHandle != NULL) {
        // vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

void StackMonitorTask::monitorTask(void *pvParameters) {
    StackMonitorTask *stackMonitor = static_cast<StackMonitorTask*>(pvParameters);

    while (true) {
        stackMonitor->readStack();
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void StackMonitorTask::readStack() {
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(_getTaskHandle);
    unsigned long currentFreeStackSpace = highWaterMark * sizeof(StackType_t);
    unsigned long usedStackSpace = _stackSize - currentFreeStackSpace;
    _usedStackPercentage = (float)usedStackSpace / _stackSize * 100.0;
    Serial.print("stacksize: ");
    Serial.println(_stackSize);
    Serial.print("used: ");
    Serial.println(_usedStackPercentage);
}

float StackMonitorTask::getUsedStackPercentage() {
    return _usedStackPercentage;
}