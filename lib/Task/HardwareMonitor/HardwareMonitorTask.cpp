#include <HardwareMonitor/HardwareMonitorTask.h>
#include <esp_heap_caps.h>
#include <WiFi.h>

HardwareMonitorTask::HardwareMonitorTask(WebSocketTask* webSocketTask) 
    : _taskHandle(NULL), _webSocketTask(webSocketTask), _freeMemKB(0), _freeMemMB(0), _freeMemPercentage(0), _isOnline(false) {
    _connectionCheckSemaphore = xSemaphoreCreateBinary();
}

void HardwareMonitorTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "HardwareMonitorTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void HardwareMonitorTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
    }
}

void HardwareMonitorTask::resumeTask() {
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle);
    }
}

void HardwareMonitorTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
    if (_connectionCheckSemaphore != NULL) {
        vSemaphoreDelete(_connectionCheckSemaphore);
        _connectionCheckSemaphore = NULL;
    }
}

TaskHandle_t HardwareMonitorTask::getTaskHandle() {
    return _taskHandle;
}

void HardwareMonitorTask::monitorTask(void *pvParameters) {
    HardwareMonitorTask *self = static_cast<HardwareMonitorTask*>(pvParameters);
    while (true) {
        self->getFreeMem();

        if (xSemaphoreTake(self->_connectionCheckSemaphore, (TickType_t)0) == pdTRUE) {
            self->checkConnectionStatus();
        }

        self->sendHardwareData();

        vTaskDelay(pdMS_TO_TICKS(_MONITOR_INTERVAL));
    }
}

void HardwareMonitorTask::getFreeMem() {
    float freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    uint32_t totalMemory = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    _freeMemKB = freeHeap / 1024.0;
    _freeMemMB = freeHeap / (1024.0 * 1024.0);
    _freeMemPercentage = ((float)freeHeap / (float)totalMemory) * 100.0;
}

void HardwareMonitorTask::checkConnectionStatus() {
    _isOnline = WiFi.status() == WL_CONNECTED;
}

void HardwareMonitorTask::sendHardwareData() {
    unsigned long currentMillis = millis();
    if (currentMillis - _lastSendTime >= _DATA_SEND_INTERVAL) {
    String jsonData = "{\"freeMemKB\": \"" + String(_freeMemKB) +
                  "\", \"freeMemMB\": \"" + String(_freeMemMB) +
                  "\", \"freeMemPercentage\": \"" + String(_freeMemPercentage, 2) +
                  "\", \"connectionStatus\": \"" + String(_isOnline ? "Public" : "Private") + "\"}";
        
        _webSocketTask->sendDataToClient(jsonData);
        _lastSendTime = currentMillis;
    }
}

void HardwareMonitorTask::triggerConnectionCheck() {
    xSemaphoreGive(_connectionCheckSemaphore);
}