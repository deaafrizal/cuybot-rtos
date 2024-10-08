#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>

LedControl ledControl;

extern int mode;
ModeSelectionTask::ModeSelectionTask() {
    taskHandle = NULL;
    lastMode = -1;
}

ModeSelectionTask::~ModeSelectionTask() {
    stopTask();
}

void ModeSelectionTask::startTask(int stackSize) {
    if (taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", stackSize, this, 2, &taskHandle);
    }
}

void ModeSelectionTask::stopTask() {
    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
    }
}

void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);

    self->webSocketTask.startTask();

    while (true) {
        if (self->lastMode != mode) {
            switch (mode) {
                case 1: // Main Page
                    Serial.println("Resuming WebSocketTask (Main Page) mode 1.");
                    ledControl.setMode(1);
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) == eSuspended) {
                        self->webSocketTask.resumeTask();
                    }
                    break;

                case 2: // Obstacle Mode
                    Serial.println("Resuming WebSocketTask for Obstacle Mode, mode 2.");
                    ledControl.setMode(2);
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) == eSuspended) {
                        self->webSocketTask.resumeTask();
                    }
                    break;

                case 3: // Auto Follow
                    ledControl.setMode(3);
                    Serial.println("Suspending WebSocketTask and starting auto follow, mode 3.");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    // Add code to start ultrasonic & IR sensor here if needed
                    break;

                case 4: // Auto Avoidance
                    Serial.println("Suspending WebSocketTask and starting auto avoidance, mode 4.");
                    ledControl.setMode(4);
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    break;

                case 5: // Tuning Page
                    Serial.println("Suspending WebSocketTask and opening TUNING page), mode 5");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    break;
                case 6: // Other Settings
                    Serial.println("Suspending WebSocketTask and opening OTA page), mode 6");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    break;

                default:
                    Serial.println("Unknown mode, no action taken.");
                    break;
            }
            self->lastMode = mode;  // Update last mode after processing
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay to avoid a tight loop
    }
}