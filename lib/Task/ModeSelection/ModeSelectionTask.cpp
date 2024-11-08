#include <ModeSelection/ModeSelectionTask.h>

extern int mode;

SemaphoreHandle_t modeChangeSemaphore;

ModeSelectionTask::ModeSelectionTask(UltrasonicTask &ultrasonicTask, LineFollowingTask &lineFollowingTask, Buzzer &buzzer, LedControl &ledControl, SpinningTask &spinningTask, AutoPatrolTask &autoPatrolTask)
    : _ultrasonicTask(ultrasonicTask), _lineFollowingTask(lineFollowingTask), _buzzer(buzzer), _ledControl(ledControl), _spinningTask(spinningTask), _autoPatrolTask(autoPatrolTask),_lastMode(1) {
    _taskHandle = NULL;
    modeChangeSemaphore = xSemaphoreCreateBinary();
    if (modeChangeSemaphore == NULL) {
        Serial.println("Error creating modeChangeSemaphore!");
    }
}

void ModeSelectionTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void ModeSelectionTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("mode selection task stopped!");
    }
}

void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);
    
    while (true) {
        if (xSemaphoreTake(modeChangeSemaphore, portMAX_DELAY) == pdTRUE) {
            self->_buzzer.beep(100);
            switch (mode) {
                case 1: // WebSocketTask only
                    Serial.println("Mode 1: Manual Control");
                    self->_ledControl.setMode(mode);
                    if (self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.stopTask();
                    }
                    if (self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.stopTask();
                    }
                    if (self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.stopTask();
                    }
                    if (self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.stopTask();
                    }
                    break;

                case 2: // WebSocketTask + UltrasonicTask
                    Serial.println("Mode 2: Obstacle Avoidance");
                    self->_ledControl.setMode(mode);
                    if (!self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.startTask();
                    }
                    if (self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.stopTask();
                    }
                    if (self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.stopTask();
                    }
                    if (self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.stopTask();
                    }
                    break;

                case 3: // LineFollowingTask only
                    Serial.println("Mode 3: Line Following");
                    self->_ledControl.setMode(mode);
                    if (self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.stopTask();
                    }
                    if (!self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.startTask();
                    }
                    if (self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.stopTask();
                    }
                    if (self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.stopTask();
                    }
                    break;

                case 4: // Patrol Mode
                    Serial.println("Mode 4: Auto Patrol");
                    self->_ledControl.setMode(mode);
                    if (!self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.startTask();
                    }
                    if (self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.stopTask();
                    }
                    if (self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.stopTask();
                    }
                    if (self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.stopTask();
                    }
                    break;

                case 5: // Spinning Mode
                    Serial.println("Mode 5: Spinning");
                    self->_ledControl.setMode(4);
                    if (!self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.startTask();
                    }
                    if (self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.stopTask();
                    }
                    if (self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.stopTask();
                    }
                    if (self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.stopTask();
                    }
                    break;

                default:
                    Serial.println("Unknown mode. No action taken.");
                    self->_ledControl.setMode(4);
                    if (self->_ultrasonicTask.getIsRunning()) {
                        self->_ultrasonicTask.stopTask();
                    }
                    if (self->_lineFollowingTask.getIsRunning()) {
                        self->_lineFollowingTask.stopTask();
                    }
                    if (self->_autoPatrolTask.getIsRunning()) {
                        self->_autoPatrolTask.stopTask();
                    }
                    if (self->_spinningTask.getIsRunning()) {
                        self->_spinningTask.stopTask();
                    }
                    break;
            }

            self->_lastMode = mode; 
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void ModeSelectionTask::triggerModeChange(int newMode) {
    mode = newMode;
    xSemaphoreGive(modeChangeSemaphore);
}