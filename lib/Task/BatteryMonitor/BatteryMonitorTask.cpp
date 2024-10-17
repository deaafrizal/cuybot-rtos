#include <BatteryMonitor/BatteryMonitorTask.h>

BatteryMonitorTask::BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor)
    : _adcPin(adcPin), _minVoltage(minVoltage), _maxVoltage(maxVoltage), _voltageDividerFactor(voltageDividerFactor), _batteryVoltage(0), _batteryPercentage(0), _taskHandle(NULL) {}

// Start the FreeRTOS task
void BatteryMonitorTask::startMonitoring() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "BatteryMonitorTask", 2048, this, 1, &_taskHandle);
    }
}

// Stop the FreeRTOS task
void BatteryMonitorTask::stopMonitoring() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

// FreeRTOS task that periodically reads the battery voltage
void BatteryMonitorTask::monitorTask(void *pvParameters) {
    BatteryMonitorTask *self = static_cast<BatteryMonitorTask *>(pvParameters);

    while (true) {
        self->readBattery();  // Read the battery voltage and percentage
        vTaskDelay(pdMS_TO_TICKS(1000));  // Delay for 1 second
    }
}

void BatteryMonitorTask::readBattery() {
    int rawAdcValue = analogRead(_adcPin);
    
    float stepUpOutputVoltage = (rawAdcValue / 4095.0) * 3.3 * _voltageDividerFactor;

    // Define the conversion factor from your step-up output to the actual battery voltage
    float conversionFactor = 1.14; // Based on your measured ratio

    // Estimate the battery voltage
    float batteryVoltage = stepUpOutputVoltage / conversionFactor;

    // Store the battery voltage
    _batteryVoltage = batteryVoltage;

    // Calculate the battery percentage based on the estimated voltage range
    _batteryPercentage = ((batteryVoltage - _minVoltage) / (_maxVoltage - _minVoltage)) * 100.0;
    _batteryPercentage = constrain(_batteryPercentage, 0, 100);  // Ensure percentage is between 0% and 100%

    // Output the battery voltage and percentage to serial (optional)
    Serial.printf("Battery Voltage: %.2fV, Battery Percentage: %.2f%%\n", _batteryVoltage, _batteryPercentage);
}

// Get the last read battery percentage
float BatteryMonitorTask::getBatteryPercentage() {
    return _batteryPercentage;
}

// Get the last read battery voltage
float BatteryMonitorTask::getBatteryVoltage() {
    return _batteryVoltage;
}