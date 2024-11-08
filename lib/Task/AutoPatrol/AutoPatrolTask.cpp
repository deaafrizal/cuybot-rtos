#include <AutoPatrol/AutoPatrolTask.h>

//sesuaikan sendiri nilai _spinSpeed, _forwardSpeed, _spinDuration, _forwardDuration jika diperlukan patroli yang lebih presisi.

AutoPatrolTask::AutoPatrolTask(MotorControl &motorControl) 
    : _motorControl(motorControl), _taskHandle(NULL), _spinSpeed(70), _forwardSpeed(45), 
      _spinDuration(785), _forwardDuration(2000), _pauseDuration(100), 
      _taskRunning(false), _state(State::MOVE_FORWARD), _lastStateChangeMillis(0) {}

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
        _motorControl.setSpeed(0, 0);
        Serial.println("Auto patrol task stopped.");
    }
}

bool AutoPatrolTask::getIsRunning() {
    return _taskRunning;
}

void AutoPatrolTask::patrolTask(void *pvParameters) {
    AutoPatrolTask *self = static_cast<AutoPatrolTask *>(pvParameters);
    unsigned long currentMillis;

    self->_lastStateChangeMillis = millis();
    while (self->_taskRunning) {
        currentMillis = millis();

        switch (self->_state) {
            case State::MOVE_FORWARD:
                if (currentMillis - self->_lastStateChangeMillis >= self->_forwardDuration) {
                    self->_motorControl.setSpeed(0, 0);
                    self->_lastStateChangeMillis = currentMillis;
                    self->_state = State::PAUSE_BEFORE_SPIN;
                } else {
                    self->_motorControl.setSpeed(self->_forwardSpeed, self->_forwardSpeed);
                }
                break;

            case State::PAUSE_BEFORE_SPIN:
                if (currentMillis - self->_lastStateChangeMillis >= self->_pauseDuration) {
                    self->_motorControl.setSpeed(0, 0);
                    self->_lastStateChangeMillis = currentMillis;
                    self->_state = State::SPIN;
                }
                break;

            case State::SPIN:
                self->_motorControl.setSpeed(-self->_spinSpeed, self->_spinSpeed);
                if (currentMillis - self->_lastStateChangeMillis >= self->_spinDuration) {
                    self->_lastStateChangeMillis = currentMillis;
                    self->_state = State::PAUSE_BEFORE_MOVE;
                }
                break;

            case State::PAUSE_BEFORE_MOVE:
                if (currentMillis - self->_lastStateChangeMillis >= self->_pauseDuration) {
                    self->_motorControl.setSpeed(0, 0);
                    self->_lastStateChangeMillis = currentMillis;
                    self->_state = State::MOVE_FORWARD;
                }
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
    Serial.println("Auto patrol task completed.");
}