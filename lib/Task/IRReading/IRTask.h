#ifndef IRTASK_H
#define IRTASK_H

#include <Arduino.h>
#include <IR/ir.h>
#include <Motor/MotorControl.h>
#include <EEPROM_config.h>

class IRTask {
public:
    IRTask(IR &ir, MotorControl &motorControl, EEPROMConfig &eepromConfig);
    void startTask();
    void stopTask(); 
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    IR &_ir;
    MotorControl &_motorControl;
    EEPROMConfig &_eepromConfig;
    static void irMeasureTask(void *_parameters);
    TaskHandle_t _taskHandle;
    uint8_t _motorMaxSpeed;
    uint8_t _motorWeight;
    const int _vdelayTime = 100;
    bool taskRunning;
};

#endif