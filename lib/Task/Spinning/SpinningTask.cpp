#include <Spinning/SpinningTask.h>

// atur sendiri kecepatan _spinSpeed (0-100).
// default 50 atau 50% dari fullspeed

SpinningTask::SpinningTask(MotorControl &motorControl) 
    : _motorControl(motorControl), _taskHandle(NULL), _spinSpeed(50), _taskRunning(false) {}

void SpinningTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(spinTask, "SpinningTask", 2048, this, 3, &_taskHandle);
    }
}

void SpinningTask::stopTask() {
    if (_taskHandle != NULL) {
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        _motorControl.setSpeed(0, 0);
        Serial.println("Spinning task stopped.");
    }
}

bool SpinningTask::getIsRunning() {
    return _taskRunning;
}

void SpinningTask::spinTask(void *pvParameters) {
    SpinningTask *self = static_cast<SpinningTask *>(pvParameters);
    
    while (self->_taskRunning) {
        self->_motorControl.setSpeed(-self->_spinSpeed, self->_spinSpeed);
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    Serial.println("Spinning task completed.");
}