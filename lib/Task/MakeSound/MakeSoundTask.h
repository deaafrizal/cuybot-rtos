#ifndef MAKESOUNDTASK_H
#define MAKESOUNDTASK_H

#include <Arduino.h>
#include <Buzzer/Buzzer.h>  // Include the Buzzer class

class MakeSound {
public:
    MakeSound();  // Constructor
    void startSuccessSoundTask(int duration);
    void startFailedSoundTask(int duration);
    void startTask();      // Stop the task
    void stopTask();      // Stop the task
    void suspendTask();   // Suspend the task
    void resumeTask();    // Resume the task

private:
    Buzzer _buzzer;         // Buzzer object to control the buzzer
    TaskHandle_t _taskHandle;  // FreeRTOS task handle
    static void successSoundTask(void *_parameters);  // Static task function for FreeRTOS
    static void failedSoundTask(void *_parameters);  // Static task function for FreeRTOS
    static void soundTask(void *_parameters);  // Static task function for FreeRTOS
};

#endif