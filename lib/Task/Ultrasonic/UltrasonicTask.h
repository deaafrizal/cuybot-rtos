#ifndef ULTRASONICTASK_H
#define ULTRASONICTASK_H

#include <Ultrasonic/Ultrasonic.h>
#include <Motor/MotorControl.h>
#include <EEPROM_config.h>

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl, EEPROMConfig &eepromConfig);
    ~UltrasonicTask();

    void startTask();
    void stopTask();
    void suspendTask();
    void resumeTask();
    
    TaskHandle_t getTaskHandle();

private:
    static void distanceMeasureTask(void *parameters);

    TaskHandle_t _taskHandle; 
    Ultrasonic &_ultrasonic;
    MotorControl &_motorControl;
    EEPROMConfig &_eepromConfig;

    const int _vdelayTime = 70;
    const float _minDistance = 0.0;
    const float _maxDistance = 17.0;
    
    bool taskRunning;
    float _distance;
};

#endif