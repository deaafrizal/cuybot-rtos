#ifndef MODESELECTIONTASK_H
#define MODESELECTIONTASK_H

#include <WebSocket/WebSocketTask.h>
#include <Motor/MotorTask.h>
#include <Ultrasonic/UltrasonicTask.h>

class ModeSelectionTask {
public:
    ModeSelectionTask(MotorTask &motor, UltrasonicTask &ultrasonic);
    ~ModeSelectionTask();

    void startTask(int stackSize = 5096);
    void stopTask();

private:
    static void modeSelectionTaskFunction(void *parameter);
    MotorTask &_motorTask;
    UltrasonicTask &_ultrasonicTask;
    TaskHandle_t taskHandle;
    int lastMode;
    WebSocketTask webSocketTask;
};

#endif