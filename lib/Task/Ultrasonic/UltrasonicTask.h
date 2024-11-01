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

// TOTAL 400KB
// ULTRASONIC -> 12KB

// 400 - 12 = 388KB


//400KB x 1024 =  409600 Bytes.
// 409600 / 4 = 102400 Word

// 1 WORD = 4 Bytes, 
// 1024 Bytes = 1KB
private:
    const uint32_t _taskStackSize = 3072; // word -> 12288 Byte -> 12KB
    const UBaseType_t _taskPriority = 5;
    int speed;
    Ultrasonic _ultrasonic;
    TaskHandle_t _taskHandle;
    
    bool _taskRunning;
    static EEPROMConfig &_eepromConfig;
    static void DistanceMeasureTask(void *parameters);
};

#endif