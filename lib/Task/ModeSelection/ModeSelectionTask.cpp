#include <ModeSelection/ModeSelectionTask.h>
#include <Arduino.h>

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
        if (mode < 0 || mode >= 6) {
            Serial.println("Invalid mode detected, ignoring...");
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }

        // Handle mode changes
        if (self->lastMode != mode) {
            switch (mode) {
                case 0: // Main Page
                    Serial.println("Resuming WebSocketTask (Main Page) mode 0.");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) == eSuspended) {
                        self->webSocketTask.resumeTask();
                    }
                    break;

                case 1: // Obstacle Mode
                    Serial.println("Resuming WebSocketTask for Obstacle Mode, mode 1.");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) == eSuspended) {
                        self->webSocketTask.resumeTask();
                    }
                    break;

                case 2: // Auto Follow
                    Serial.println("Suspending WebSocketTask and starting auto follow, mode 2.");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    // Add code to start ultrasonic & IR sensor here if needed
                    break;

                case 3: // Auto Avoidance
                    Serial.println("Suspending WebSocketTask and starting auto avoidance, mode 3.");
                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    break;

                case 4: // Tuning Page
                case 5: // Other Settings
                    Serial.println("Suspending WebSocketTask and opening settings page (tuning, OTA & reboot page), mode 4/5.");
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