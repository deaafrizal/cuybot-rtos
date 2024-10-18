#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>
#include <Buzzer/Buzzer.h>
#include "freertos/semphr.h"

LedControl ledControl;
Buzzer buzzer(1);
extern int mode;

// Create the semaphore
SemaphoreHandle_t modeChangeSemaphore;

ModeSelectionTask::ModeSelectionTask(MotorTask &motor, UltrasonicTask &ultrasonicTask, IRTask &irTask)
    : _motorTask(motor), _ultrasonicTask(ultrasonicTask), _irTask(irTask), _lastMode(-1){
        _taskHandle = NULL;
        buzzer.begin();

        // Initialize the semaphore
        modeChangeSemaphore = xSemaphoreCreateBinary();
        if (modeChangeSemaphore == NULL) {
            Serial.println("Error creating modeChangeSemaphore!");
        }
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

// Task function using semaphore to detect mode change
void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);

    // Store task handles once to reduce overhead
    TaskHandle_t wsTaskHandle = self->_webSocketTask.getTaskHandle();
    TaskHandle_t ultrasonicTaskHandle = self->_ultrasonicTask.getTaskHandle();
    TaskHandle_t irTaskHandle = self->_irTask.getTaskHandle();

    // Start necessary tasks initially
    self->_webSocketTask.startTask();
    self->_ultrasonicTask.startTask();
    self->_irTask.startTask();

    // Suspend irrelevant tasks at startup
    self->_ultrasonicTask.suspendTask();
    self->_irTask.suspendTask();

    while (true) {
        // Wait for the semaphore to be given when mode changes
        if (xSemaphoreTake(modeChangeSemaphore, portMAX_DELAY) == pdTRUE) {
            buzzer.beep(100);  // Beep on mode change

            // Detect mode change and act accordingly
            switch (mode) {
                case 1: // WebSocketTask only
                    Serial.println("Mode 1: WebSocketTask only");
                    ledControl.setMode(1);

                    // Enable WebSocketTask, disable others
                    resumeTaskIfSuspended(wsTaskHandle, self->_webSocketTask);
                    suspendTaskIfRunning(ultrasonicTaskHandle, self->_ultrasonicTask);
                    suspendTaskIfRunning(irTaskHandle, self->_irTask);
                    break;

                case 2: // WebSocketTask + UltrasonicTask
                    Serial.println("Mode 2: WebSocketTask + UltrasonicTask");
                    ledControl.setMode(2);

                    // Enable WebSocketTask and UltrasonicTask
                    resumeTaskIfSuspended(wsTaskHandle, self->_webSocketTask);
                    resumeTaskIfSuspended(ultrasonicTaskHandle, self->_ultrasonicTask);
                    suspendTaskIfRunning(irTaskHandle, self->_irTask);
                    break;

                case 3: // IRTask only (Line Following)
                    Serial.println("Mode 3: IRTask only");
                    ledControl.setMode(3);

                    // Enable IRTask, disable others
                    resumeTaskIfSuspended(irTaskHandle, self->_irTask);
                    suspendTaskIfRunning(ultrasonicTaskHandle, self->_ultrasonicTask);
                    suspendTaskIfRunning(wsTaskHandle, self->_webSocketTask);
                    break;

                case 4: // Patrol Mode or Tuning
                    Serial.println("Mode 4: Patrol Mode");
                    ledControl.setMode(4);
                    break;

                // Add more modes here...

                default:
                    Serial.println("Unknown mode, no action taken.");
                    break;
            }

            self->_lastMode = mode;  // Store last mode
        }
    }
}

// Function to signal mode change
void triggerModeChange(int newMode) {
    mode = newMode;
    xSemaphoreGive(modeChangeSemaphore);  // Signal the semaphore to handle the new mode
}