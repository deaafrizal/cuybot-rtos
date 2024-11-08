#ifndef ULTRASONIC_TASK_H
#define ULTRASONIC_TASK_H

#include <Arduino.h>
#include <Ultrasonic/Ultrasonic.h>
#include <Motor/MotorControl.h>

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl);
    void startTask();
    bool getIsRunning();
    void stopTask();

private:
    Ultrasonic &_ultrasonic;
    MotorControl &_motorControl;
    TaskHandle_t _taskHandle;

    const uint32_t _taskStackSize = 3072; // word -> 12288 Byte -> 12KB
    const UBaseType_t _taskPriority = 5;
    int _initSpeed;
    bool _taskRunning;

    static void DistanceMeasureTask(void *parameters);
};

#endif