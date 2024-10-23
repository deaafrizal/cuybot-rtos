#ifndef BATTERY_MONITOR_TASK_H
#define BATTERY_MONITOR_TASK_H

#include <Arduino.h>

class BatteryMonitorTask {
public:
    BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor);
    
    void startMonitoring();
    void stopMonitoring();
    float getBatteryPercentage();
    float getBatteryVoltage();

private:
    uint8_t _adcPin;
    float _minVoltage;
    float _maxVoltage;
    float _voltageDividerFactor;
    float _batteryVoltage;
    float _batteryPercentage;
    TaskHandle_t _taskHandle;

    static void monitorTask(void *pvParameters);
    void readBattery();
};

#endif