#include <AutoPatrol/AutoPatrolTask.h>

AutoPatrolTask::AutoPatrolTask(MotorControl &motorControl) 
    : _motorControl(motorControl), _taskHandle(NULL), _spinSpeed(90), _forwardSpeed(65), _spinDuration(700), _forwardDuration(2200), _taskRunning(false) {}

void AutoPatrolTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(patrolTask, "AutoPatrolTask", 2048, this, 3, &_taskHandle);
    }
}

void AutoPatrolTask::stopTask() {
    if (_taskHandle != NULL) {
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        _motorControl.stop();
        Serial.println("Auto patrol task stopped.");
    }
}

bool AutoPatrolTask::getIsRunning() {
    return _taskRunning;
}

void AutoPatrolTask::patrolTask(void *pvParameters) {
    AutoPatrolTask *self = static_cast<AutoPatrolTask *>(pvParameters);

    while (self->_taskRunning) {
        // Bergerak maju dengan kecepatan forwardSpeed
        self->_motorControl.setSpeed(self->_forwardSpeed, self->_forwardSpeed);
        vTaskDelay(pdMS_TO_TICKS(self->_forwardDuration));

        // Berhenti 500ms (setengah detik) sebelum cuybot berputar
        self->_motorControl.stop();
        vTaskDelay(pdMS_TO_TICKS(500));

        // Berputar dengan kecepatan spinSpeed (satu motor maju, satu motor mundur)
        self->_motorControl.setSpeed(self->_spinSpeed, -self->_spinSpeed);
        vTaskDelay(pdMS_TO_TICKS(self->_spinDuration));

        // Berhenti 500ms (setengah detik) sebelum mulai patroli lagi
        self->_motorControl.stop();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    Serial.println("Auto patrol task completed.");
}