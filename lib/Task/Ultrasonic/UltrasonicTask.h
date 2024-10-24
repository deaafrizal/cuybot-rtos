#ifndef ULTRASONIC_TASK_H
#define ULTRASONIC_TASK_H

#include <Arduino.h>
#include <Ultrasonic/Ultrasonic.h>
#include <Motor/MotorTask.h>

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonic, MotorTask &motorTask);
    void startTask();
    void stopTask();

private:
    static void DistanceMeasureTask(void *parameters);
    MotorTask _motorTask;
    Ultrasonic _ultrasonic;
    TaskHandle_t _taskHandle;
    bool _taskRunning;
};

#endif