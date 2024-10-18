#ifndef BATTERY_MONITOR_TASK_H
#define BATTERY_MONITOR_TASK_H

#include <Arduino.h>

class BatteryMonitorTask {
public:
    BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor);
    
    void startMonitoring();          // Start the FreeRTOS task for monitoring battery
    void stopMonitoring();           // Stop the FreeRTOS task
    float getBatteryPercentage();    // Get the last read battery percentage
    float getBatteryVoltage();       // Get the last read battery voltage

private:
    uint8_t _adcPin;
    float _minVoltage;
    float _maxVoltage;
    float _voltageDividerFactor;
    float _batteryVoltage;
    float _batteryPercentage;
    TaskHandle_t _taskHandle;

    static void monitorTask(void *pvParameters);  // FreeRTOS task to monitor battery
    void readBattery();                           // Function to read and calculate battery voltage/percentage
};

#endif