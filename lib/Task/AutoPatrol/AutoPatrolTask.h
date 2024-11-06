#ifndef AUTOPATROLTASK_H
#define AUTOPATROLTASK_H

#include <Arduino.h>
#include <Motor/MotorControl.h>

class AutoPatrolTask {
public:
    AutoPatrolTask(MotorControl &motorControl);
    
    void startTask();
    void stopTask();
    bool getIsRunning();

private:
    static void patrolTask(void *pvParameters);

    MotorControl &_motorControl;
    TaskHandle_t _taskHandle;
    int _spinSpeed;
    int _forwardSpeed;
    int _spinDuration;
    int _forwardDuration;
    bool _taskRunning;
};

#endif