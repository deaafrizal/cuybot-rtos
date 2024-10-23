#include <HardwareMonitor/HardwareMonitorTask.h>
#include <esp_heap_caps.h>

HardwareMonitorTask::HardwareMonitorTask(WebSocketTask* webSocketTask) 
    : _taskHandle(NULL), _webSocketTask(webSocketTask), _freeMemKB(0), _freeMemMB(0), _freeMemPercentage(0) {
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
}

TaskHandle_t HardwareMonitorTask::getTaskHandle() {
    return _taskHandle;
}

void HardwareMonitorTask::monitorTask(void *pvParameters) {
    HardwareMonitorTask *self = static_cast<HardwareMonitorTask*>(pvParameters);
    while (true) {
        // Gather memory data every _MONITOR_INTERVAL
        self->getFreeMem();

        // Only send memory data at a separate interval (_DATA_SEND_INTERVAL)
        self->sendFreeMemData();

        // Delay until the next monitoring cycle
        vTaskDelay(pdMS_TO_TICKS(_MONITOR_INTERVAL));
    }
}

// Gather free memory data and calculate percentage
void HardwareMonitorTask::getFreeMem() {
    float freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    uint32_t totalMemory = heap_caps_get_total_size(MALLOC_CAP_8BIT);

    _freeMemKB = freeHeap / 1024.0;
    _freeMemMB = freeHeap / (1024.0 * 1024.0);

    _freeMemPercentage = ((float)freeHeap / (float)totalMemory) * 100.0;
}

// Send free memory data via WebSocket at a separate interval
void HardwareMonitorTask::sendFreeMemData() {
    unsigned long currentMillis = millis();
    if (currentMillis - _lastSendTime >= _DATA_SEND_INTERVAL) {
        // Send the memory data as a JSON object
        String jsonData = "{\"freeMemKB\": " + String(_freeMemKB) +
                          ", \"freeMemMB\": " + String(_freeMemMB) +
                          ", \"freeMemPercentage\": " + String(_freeMemPercentage, 2) + "}";

        _webSocketTask->sendStackDataToClient(jsonData);
        _lastSendTime = currentMillis;  // Update last send time
    }
}