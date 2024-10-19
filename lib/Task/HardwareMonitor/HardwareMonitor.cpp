#include <HardwareMonitor/HardwareMonitor.h>
#include <esp_heap_caps.h>

HardwareMonitor::HardwareMonitor() : _taskHandle(nullptr) {}

void HardwareMonitor::startTask() {
    if (_taskHandle == nullptr) {
        xTaskCreate(monitorTask, "HWMonitor Task", 1248, this, 2, &_taskHandle);
        if (_taskHandle != nullptr) {
            Serial.println("HardwareMonitor task started successfully.");
        } else {
            Serial.println("Failed to start HardwareMonitor task.");
        }
    }
}

void HardwareMonitor::monitorTask(void *parameter) {
    HardwareMonitor *self = static_cast<HardwareMonitor*>(parameter);

    while (true) {
        size_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
        size_t totalHeap = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
        
        float freeHeapKB = freeHeap / 1024.0;
        float totalHeapKB = totalHeap / 1024.0;

        Serial.printf("Free RAM: %.2f KB, Total RAM: %.2f KB\n", freeHeapKB, totalHeapKB);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void HardwareMonitor::stopTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("HardwareMonitor task stopped.");
    }
}

TaskHandle_t HardwareMonitor::getTaskHandle() {
    return _taskHandle;
}