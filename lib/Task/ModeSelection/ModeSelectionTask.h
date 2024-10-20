#ifndef MODESELECTIONTASK_H
#define MODESELECTIONTASK_H

#include <Motor/MotorTask.h>
#include <Ultrasonic/UltrasonicTask.h>
#include <IRReading/IRTask.h>

class ModeSelectionTask {
public:
    ModeSelectionTask(MotorTask &motorTask, UltrasonicTask &ultrasonic, IRTask &irTask);
    ~ModeSelectionTask();

    void startTask(int stackSize = 5096);
    void stopTask();
    static void triggerModeChange(int newMode);

private:
    TaskHandle_t _taskHandle;
    
    MotorTask &_motorTask;
    UltrasonicTask &_ultrasonicTask;
    IRTask &_irTask;

    int _lastMode;
    static void modeSelectionTaskFunction(void *parameter);
};

#endif