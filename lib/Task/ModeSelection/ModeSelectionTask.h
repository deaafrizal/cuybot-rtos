#ifndef MODESELECTIONTASK_H
#define MODESELECTIONTASK_H

#include <WebSocket/WebSocketTask.h>

class ModeSelectionTask {
public:
    ModeSelectionTask();
    ~ModeSelectionTask();

    void startTask(int stackSize = 4096);
    void stopTask();

private:
    static void modeSelectionTaskFunction(void *parameter);

    TaskHandle_t taskHandle;
    int lastMode;
    WebSocketTask webSocketTask;  // WebSocketTask instance
};

#endif