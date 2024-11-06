#ifndef MODESELECTIONTASK_H
#define MODESELECTIONTASK_H

#include <Ultrasonic/UltrasonicTask.h>
#include <IRReading/IRTask.h>
#include <Buzzer/Buzzer.h>
#include <freertos/semphr.h>
#include <Spinning/SpinningTask.h>
#include <AutoPatrol/AutoPatrolTask.h>

class ModeSelectionTask {
public:
    ModeSelectionTask(UltrasonicTask &ultrasonic, IRTask &irTask, Buzzer &buzzer, SpinningTask &spinningTask, AutoPatrolTask &autoPatrolTask);

    void startTask();
    void stopTask();
    static void triggerModeChange(int newMode);

private:
    TaskHandle_t _taskHandle;
    const uint32_t _taskStackSize = 4096;
    const UBaseType_t _taskPriority = 8;
    Buzzer& _buzzer;
    UltrasonicTask &_ultrasonicTask;
    IRTask &_irTask;
    SpinningTask &_spinningTask;
    AutoPatrolTask &_autoPatrolTask;

    int _lastMode;
    static void modeSelectionTaskFunction(void *parameter);
};

#endif