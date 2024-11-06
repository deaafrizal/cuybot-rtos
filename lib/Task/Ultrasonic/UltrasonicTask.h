#ifndef ULTRASONIC_TASK_H
#define ULTRASONIC_TASK_H

#include <Arduino.h>
#include <Ultrasonic/Ultrasonic.h>
#include <EEPROM_config.h>

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonic);
    void startTask();
    bool getIsRunning();
    void stopTask();

private:
    const uint32_t _taskStackSize = 3072; // 1 words -> 4 Byte, 3072 words = 12KB
    const UBaseType_t _taskPriority = 5;
    int speed;
    Ultrasonic _ultrasonic;
    TaskHandle_t _taskHandle;
    
    bool _taskRunning;
    static EEPROMConfig &_eepromConfig;
    static void DistanceMeasureTask(void *parameters);
};

#endif