#ifndef ULTRASONIC_TASK_H
#define ULTRASONIC_TASK_H

#include <Arduino.h>
#include <Ultrasonic/Ultrasonic.h>
#include <Motor/MotorTask.h>

#define ULTRASONIC_FILTER_SIZE 5

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonicSensor, MotorTask &motorTask);
    void startTask();
    void stopTask();
    void suspendTask();
    void resumeTask(); 
    bool taskRunning; 
    TaskHandle_t getTaskHandle();


private:
    Ultrasonic &_ultrasonic;
    MotorTask &_motorTask;
    long _distance;

    TaskHandle_t _taskHandle;
    const int _minDistance = 0;
    const int _maxDistance = 30;
    const int _timeoutPeriod = 15000;
    const int _vdelayTime = 70;
    static void distanceMeasureTask(void *_parameters);
};

#endif