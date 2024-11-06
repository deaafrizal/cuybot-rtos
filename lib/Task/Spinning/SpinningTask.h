#ifndef SPINNINGTASK_H
#define SPINNINGTASK_H

#include <Arduino.h>
#include <Motor/MotorControl.h>

class SpinningTask {
public:
    SpinningTask(MotorControl &motorControl);
    
    void startTask();
    void stopTask();
    bool getIsRunning();

private:
    static void spinTask(void *pvParameters);
    MotorControl &_motorControl;
    TaskHandle_t _taskHandle;
    int _spinSpeed;
    bool _taskRunning;
};

#endif // SPINNINGTASK_H