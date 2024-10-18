#ifndef ULTRASONICTASK_H
#define ULTRASONICTASK_H

#include <Ultrasonic/Ultrasonic.h>
#include <Motor/MotorControl.h>

class UltrasonicTask {
public:
    UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl);

    // Task management functions
    void startTask();
    void stopTask();
    void suspendTask();
    void resumeTask();
    TaskHandle_t getTaskHandle();

private:
    // Task function to measure distance
    static void distanceMeasureTask(void *parameters);

    Ultrasonic &_ultrasonic;  // Ultrasonic sensor object
    MotorControl &_motorControl;  // MotorControl object
    float _distance;  // Measured distance
    bool taskRunning;  // Flag to check task state
    TaskHandle_t _taskHandle;  // Handle for the FreeRTOS task
    const int _vdelayTime = 100;  // Delay between measurements (ms)
    const float _minDistance = 10.0;  // Minimum distance for obstacle detection
    const float _maxDistance = 50.0;  // Maximum distance for obstacle detection
};

#endif // ULTRASONICTASK_H