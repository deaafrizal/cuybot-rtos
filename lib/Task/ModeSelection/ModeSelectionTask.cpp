#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>
#include <Buzzer/Buzzer.h>
#include <freertos/semphr.h>

LedControl ledControl;
Buzzer buzzer(1);
extern int mode;

SemaphoreHandle_t modeChangeSemaphore;

ModeSelectionTask::ModeSelectionTask(MotorTask &motorTask, UltrasonicTask &ultrasonicTask, IRTask &irTask)
    : _motorTask(motorTask), _ultrasonicTask(ultrasonicTask), _irTask(irTask), _lastMode(1){
        _taskHandle = NULL;
        buzzer.begin();

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

void ModeSelectionTask::modeSelectionTaskFunction(void *parameter) {
    ModeSelectionTask *self = static_cast<ModeSelectionTask*>(parameter);

    // Ensure tasks are started and suspended initially
    self->_ultrasonicTask.startTask();
    self->_irTask.startTask();

    TaskHandle_t ultrasonicTaskHandle = self->_ultrasonicTask.getTaskHandle();
    TaskHandle_t irTaskHandle = self->_irTask.getTaskHandle();

    while (true) {
        Serial.print("Current mode: ");
        Serial.println(mode);

        if (xSemaphoreTake(modeChangeSemaphore, portMAX_DELAY) == pdTRUE) {
            buzzer.beep(100);  // Beep on mode change

            // Suspend both tasks before changing modes
            if (ultrasonicTaskHandle != nullptr) {
                Serial.println("Suspending Ultrasonic task...");
                vTaskSuspend(ultrasonicTaskHandle);
                Serial.println("Ultrasonic task suspended.");
            } else {
                Serial.println("Ultrasonic task handle is null, cannot suspend.");
            }

            if (irTaskHandle != nullptr) {
                Serial.println("Suspending IR task...");
                vTaskSuspend(irTaskHandle);
                Serial.println("IR task suspended.");
            } else {
                Serial.println("IR task handle is null, cannot suspend.");
            }

            // Handle mode changes and resume tasks accordingly
            switch (mode) {
                case 1: // WebSocketTask only
                    Serial.println("Mode 1: WebSocketTask only. No tasks resumed.");
                    ledControl.setMode(1);
                    break;

                case 2: // WebSocketTask + UltrasonicTask
                    Serial.println("Mode 2: WebSocketTask + UltrasonicTask");
                    ledControl.setMode(2);
                    if (ultrasonicTaskHandle != nullptr) {
                        Serial.println("Resuming Ultrasonic task...");
                        vTaskResume(ultrasonicTaskHandle);
                        Serial.println("Ultrasonic task resumed.");
                    } else {
                        Serial.println("Ultrasonic task handle is null, cannot resume.");
                    }
                    break;

                case 3: // IRTask only (Line Following)
                    Serial.println("Mode 3: IRTask only (Line Following)");
                    ledControl.setMode(3);
                    if (irTaskHandle != nullptr) {
                        Serial.println("Resuming IR task...");
                        vTaskResume(irTaskHandle);
                        Serial.println("IR task resumed.");
                    } else {
                        Serial.println("IR task handle is null, cannot resume.");
                    }
                    break;

                case 4: // Patrol Mode or Tuning
                    Serial.println("Mode 4: Patrol Mode");
                    ledControl.setMode(4);
                    // Additional logic for Patrol Mode
                    break;

                default:
                    Serial.println("Unknown mode. No action taken.");
                    break;
            }

            self->_lastMode = mode;  // Store last mode
        }
    }
}

// Function to signal mode change
void ModeSelectionTask::triggerModeChange(int newMode) {
    mode = newMode;
    xSemaphoreGive(modeChangeSemaphore);  // Signal the semaphore to handle the new mode
}