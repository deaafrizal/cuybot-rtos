#ifndef LINEFOLLOWINGTASK_H
#define LINEFOLLOWINGTASK_H

#include <Arduino.h>
#include <IR/IR.h>
#include <Motor/MotorControl.h>
#include <EEPROM_config.h>

class LineFollowingTask {
public:
    LineFollowingTask(IR &ir, MotorControl &motorControl);
    void startTask();
    void stopTask(); 
    bool getIsRunning();
    TaskHandle_t getTaskHandle();

private:
    IR &_ir;
    MotorControl &_motorControl;
    TaskHandle_t _taskHandle;

    const uint32_t _taskStackSize = 4096;
    const UBaseType_t _taskPriority = 6;

    static EEPROMConfig &_eepromConfig;
    static void irMeasureTask(void *_parameters);
    
    uint8_t motorMaxSpeed;
    uint8_t motorWeight;
    float _vdelayTime = 5.0;
    bool _taskRunning;
};

#endif