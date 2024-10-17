#ifndef MODESELECTIONTASK_H
#define MODESELECTIONTASK_H

#include <WebSocket/WebSocketTask.h>
#include <Motor/MotorTask.h>
#include <Ultrasonic/UltrasonicTask.h>
#include <IRReading/IRTask.h>

class ModeSelectionTask {
public:
    ModeSelectionTask(MotorTask &motor, UltrasonicTask &ultrasonic, IRTask &irTask);
    ~ModeSelectionTask();

    void startTask(int stackSize = 5096);
    void stopTask();

private:
    static void modeSelectionTaskFunction(void *parameter);
    MotorTask &_motorTask;
    UltrasonicTask &_ultrasonicTask;
    IRTask &_irTask;
    TaskHandle_t _taskHandle;
    int _lastMode;
    WebSocketTask _webSocketTask;
};

#endif