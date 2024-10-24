#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>

LedControl ledControl;
extern int mode;

SemaphoreHandle_t modeChangeSemaphore;

ModeSelectionTask::ModeSelectionTask(UltrasonicTask &ultrasonicTask, IRTask &irTask, Buzzer &buzzer)
    : _ultrasonicTask(ultrasonicTask), _irTask(irTask), _buzzer(buzzer), _lastMode(1) {
    _taskHandle = NULL;
    modeChangeSemaphore = xSemaphoreCreateBinary();
    if (modeChangeSemaphore == NULL) {
        Serial.println("Error creating modeChangeSemaphore!");
    }else {
        Serial.println("modeChangeSemaphore created!");
    }
}

void ModeSelectionTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", 4096, this, 2, &_taskHandle);
    }
}

void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);
    
    while (true) {
        if (xSemaphoreTake(modeChangeSemaphore, portMAX_DELAY) == pdTRUE) {
            self->_buzzer.beep(100);
            switch (mode) {
                case 1: // WebSocketTask only
                    Serial.println("Mode 1: WebSocketTask only. No tasks resumed.");
                    ledControl.setMode(1);
                    self->_ultrasonicTask.stopTask();
                    break;

                case 2: // WebSocketTask + UltrasonicTask
                    Serial.println("Mode 2: WebSocketTask + UltrasonicTask");
                    ledControl.setMode(2);
                    self->_ultrasonicTask.startTask();
                    break;

                case 3: // IRTask only (Line Following)
                    Serial.println("Mode 3: IRTask only (Line Following)");
                    ledControl.setMode(3);
                    self->_ultrasonicTask.stopTask();
                    break;

                case 4: // Patrol Mode or Tuning
                    Serial.println("Mode 4: Patrol Mode");
                    ledControl.setMode(4);
                    self->_ultrasonicTask.stopTask(); // Suspend ultrasonic task
                    break;

                default:
                    Serial.println("Unknown mode. No action taken.");
                    self->_ultrasonicTask.stopTask();
                    break;
            }

            self->_lastMode = mode; 
        }
    }
}

void ModeSelectionTask::triggerModeChange(int newMode) {
    mode = newMode;
    xSemaphoreGive(modeChangeSemaphore);
}