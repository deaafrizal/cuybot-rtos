#ifndef IRTASK_H
#define IRTASK_H

#include <Arduino.h>
#include <IR/ir.h>
#include <Motor/MotorDriver.h>
#include <EEPROM_config.h>

class IRTask {
public:
    IRTask(IR &ir, MotorDriver &rightMotor, MotorDriver &leftMotor);
    void startTask();
    void stopTask(); 
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    const uint32_t _taskStackSize = 4096;
    const UBaseType_t _taskPriority = 6;
    
    IR &_ir;
    MotorDriver &_rightMotor;
    MotorDriver &_leftMotor;

    static EEPROMConfig &_eepromConfig;
    static void irMeasureTask(void *_parameters);
    TaskHandle_t _taskHandle;
    
    uint8_t motorMaxSpeed;
    uint8_t motorWeight;
    static const int _vdelayTime = 100;
    bool _taskRunning;
};

#endif