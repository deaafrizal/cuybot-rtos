#include <BatteryMonitor/BatteryMonitorTask.h>

BatteryMonitorTask::BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor)
    : _adcPin(adcPin), _minVoltage(minVoltage), _maxVoltage(maxVoltage), _voltageDividerFactor(voltageDividerFactor), _batteryVoltage(0), _batteryPercentage(0), _taskHandle(NULL) {}

void BatteryMonitorTask::startMonitoring() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "BatteryMonitorTask", 2048, this, 1, &_taskHandle);
    }
}

void BatteryMonitorTask::stopMonitoring() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

void BatteryMonitorTask::monitorTask(void *pvParameters) {
    BatteryMonitorTask *self = static_cast<BatteryMonitorTask *>(pvParameters);

    while (true) {
        self->readBattery();
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

void BatteryMonitorTask::readBattery() {
    int rawAdcValue = analogRead(_adcPin);
    
    float stepUpOutputVoltage = (rawAdcValue / 4095.0) * _minVoltage * _voltageDividerFactor;
    float conversionFactor = 1.14;
    float batteryVoltage = stepUpOutputVoltage / conversionFactor;

    _batteryVoltage = batteryVoltage;

    _batteryPercentage = ((batteryVoltage - _minVoltage) / (_maxVoltage - _minVoltage)) * 100.0;
    _batteryPercentage = constrain(_batteryPercentage, 0, 100);
}

float BatteryMonitorTask::getBatteryPercentage() {
    return _batteryPercentage;
}

float BatteryMonitorTask::getBatteryVoltage() {
    return _batteryVoltage;
}