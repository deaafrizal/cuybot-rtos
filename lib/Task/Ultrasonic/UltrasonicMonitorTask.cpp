#include <Ultrasonic/UltrasonicMonitorTask.h>

UltrasonicMonitorTask::UltrasonicMonitorTask(Ultrasonic &ultrasonicSensor, UltrasonicTask &ultrasonicTask)
    : _ultrasonic(ultrasonicSensor), _ultrasonicTask(ultrasonicTask), _taskHandle(NULL) {}

void UltrasonicMonitorTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(
            monitorTask,
            "MonitorUltrasonic",
            2048,
            this,
            5,
            &_taskHandle
        );
    }
}

void UltrasonicMonitorTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

TaskHandle_t UltrasonicMonitorTask::getTaskHandle() {
    return _taskHandle;
}


void UltrasonicMonitorTask::monitorTask(void *_parameters) {
    UltrasonicMonitorTask *self = static_cast<UltrasonicMonitorTask *>(_parameters);

    int noEchoTime = 0;
    const int maxNoEchoTime = 10000;

    while (true) {
        if (!self->_ultrasonicTask.taskRunning) {
            long distance = self->_ultrasonic.getDistance();

            if (distance != -1) {
                Serial.println("Echo detected again. Restarting ultrasonic task...");
                self->_ultrasonicTask.startTask();
                noEchoTime = 0;
            } else {
                noEchoTime += 5000;
                Serial.printf("Waiting for ultrasonic sensor to power up... (%d ms of %d ms)\n", noEchoTime, maxNoEchoTime);
            }
        }

        if (noEchoTime >= maxNoEchoTime) {
            Serial.println("No echo detected for 10 seconds. Terminating monitor task...");
            vTaskDelete(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}