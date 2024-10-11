#ifndef IRTASK_H
#define IRTASK_H

#include <Arduino.h>
#include <IR/ir.h>
#include <Motor/MotorTask.h>

class IRTask {
public:
    IRTask(IR &ir, MotorTask &motorTask);
    void startTask();
    void stopTask(); 
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    IR &_ir;
    MotorTask &_motorTask;
    static void irMeasureTask(void *_parameters);
    TaskHandle_t _taskHandle;
    
    const int _vdelayTime = 100;
    bool taskRunning;
};

#endif