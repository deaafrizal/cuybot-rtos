#ifndef BATTERY_MONITOR_TASK_H
#define BATTERY_MONITOR_TASK_H

#include <Arduino.h>
#include <Buzzer/Buzzer.h>
#include <WebSocket/WebSocketTask.h>

class BatteryMonitorTask {
public:
    BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor, Buzzer &buzzer, WebSocketTask* webSocketTask);
    
    void startMonitoring();
    void stopMonitoring();
    void sendBatteryData();

private:
    const uint32_t _taskStackSize = 2048;
    const UBaseType_t _taskPriority = 2;
    
    TaskHandle_t _taskHandle;
    WebSocketTask* _webSocketTask;
    Buzzer& _buzzer;

    uint8_t _adcPin;
    float _minVoltage;
    float _maxVoltage;
    float _voltageDividerFactor;
    float _batteryVoltage;
    float _batteryPercentage;
    unsigned long _lastSendTime = 0;
    static const unsigned long _DATA_SEND_INTERVAL = 1500;
    static const unsigned long _MONITOR_INTERVAL = 1000;
    
    static void monitorTask(void *pvParameters);

    void readBattery();
    void playBatteryLowNotification();
};

#endif