#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>
#include <Buzzer/Buzzer.h>

LedControl ledControl;
Buzzer buzzer(1);
extern int mode;

ModeSelectionTask::ModeSelectionTask(MotorTask &motor, UltrasonicTask &ultrasonicTask, IRTask &irTask)
    : _motorTask(motor), _ultrasonicTask(ultrasonicTask), _irTask(irTask), _lastMode(-1){
        _taskHandle = NULL;
        buzzer.begin();
    }

ModeSelectionTask::~ModeSelectionTask() {
    stopTask();
}

void ModeSelectionTask::startTask(int stackSize) {
    if (_taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", stackSize, this, 5, &_taskHandle);
    }
}

void ModeSelectionTask::stopTask() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

bool isTaskSuspended(TaskHandle_t taskHandle) {
    return (taskHandle != NULL) && (eTaskGetState(taskHandle) == eSuspended);
}

template<typename T>
void resumeTaskIfSuspended(TaskHandle_t taskHandle, T &task) {
    if (isTaskSuspended(taskHandle)) {
        task.resumeTask();
    }
}

template<typename T>
void suspendTaskIfRunning(TaskHandle_t taskHandle, T &task) {
    if (!isTaskSuspended(taskHandle)) {
        task.suspendTask();
    }
}

void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);

    self->_webSocketTask.startTask();
    self->_ultrasonicTask.startTask();
    self->_irTask.startTask();
    self->_ultrasonicTask.suspendTask();
    self->_irTask.suspendTask();

    while (true) {
        if (self->_lastMode != mode) {
            switch (mode) {
                case 1: // Main Page (WebSocketTask only)
                    Serial.println("Mode 1: WebSocketTask only");
                    ledControl.setMode(1);

                    // Resume WebSocketTask and suspend others
                    resumeTaskIfSuspended(self->_webSocketTask.getTaskHandle(), self->_webSocketTask);
                    suspendTaskIfRunning(self->_ultrasonicTask.getTaskHandle(), self->_ultrasonicTask);
                    suspendTaskIfRunning(self->_irTask.getTaskHandle(), self->_irTask);

                    buzzer.beep(100);
                    break;

                case 2: // Obstacle Mode (WebSocketTask + UltrasonicTask)
                    Serial.println("Mode 2: WebSocketTask + UltrasonicTask");
                    ledControl.setMode(2);

                    resumeTaskIfSuspended(self->_webSocketTask.getTaskHandle(), self->_webSocketTask);
                    resumeTaskIfSuspended(self->_ultrasonicTask.getTaskHandle(), self->_ultrasonicTask);
                    suspendTaskIfRunning(self->_irTask.getTaskHandle(), self->_irTask);

                    buzzer.beep(200);
                    break;

                case 3: // Automove Line Following Mode (IRTask only)
                    Serial.println("Mode 3: IRTask only");
                    ledControl.setMode(3);

                    resumeTaskIfSuspended(self->_irTask.getTaskHandle(), self->_irTask);
                    suspendTaskIfRunning(self->_ultrasonicTask.getTaskHandle(), self->_ultrasonicTask);
                    suspendTaskIfRunning(self->_webSocketTask.getTaskHandle(), self->_webSocketTask);

                    buzzer.beep(300);
                    break;

                case 4: // tune
                    Serial.println("Mode 4: patrol mode");
                    ledControl.setMode(4);

                    buzzer.beep(400);
                    break;

                case 5: // Tuning Page (Stop All Tasks)
                    Serial.println("Mode 5: Tuning Page");

                    buzzer.beep(500);
                    break;

                case 6: // Other Settings (Stop All Tasks)
                    Serial.println("Mode 6: Other Settings");

                    buzzer.beep(600);
                    break;

                default:
                    Serial.println("Unknown mode, no action taken.");
                    break;
            }
            self->_lastMode = mode;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}