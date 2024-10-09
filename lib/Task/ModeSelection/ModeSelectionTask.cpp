#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>
#include <Ultrasonic/UltrasonicTask.h>
#include <Ultrasonic/UltrasonicMonitorTask.h>
#include <IR/IR.h>
#include <IRReading/IRTask.h>
#include <Buzzer/Buzzer.h>

LedControl ledControl;
// IR ir;
// IRTask irTask(ir);
Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic);
UltrasonicMonitorTask ultrasonicMonitorTask(ultrasonic, ultrasonicTask);

Buzzer buzzer(1);
extern int mode;

ModeSelectionTask::ModeSelectionTask() {
    Serial.println("ModeSelection Task initialize...");
    taskHandle = NULL;
    lastMode = -1;
    ultrasonic.begin();
    buzzer.begin();
}

ModeSelectionTask::~ModeSelectionTask() {
        Serial.println("ModeSelection Task cleanup...");

    stopTask();
}

void ModeSelectionTask::startTask(int stackSize) {
    Serial.println("ModeSelection Task starting up...");
    if (taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", stackSize, this, 3, &taskHandle);
    }
}

void ModeSelectionTask::stopTask() {
    Serial.println("ModeSelection Task stopped...");
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

                    if (ultrasonicTask.getTaskHandle() != NULL) {
                        if (eTaskGetState(ultrasonicTask.getTaskHandle()) != eSuspended) {
                            ultrasonicTask.stopTask();
                            ultrasonicMonitorTask.stopTask();
                            Serial.println("UltrasonicTask and UltrasonicMonitorTask stopped.");
                        }
                    }
                    buzzer.beep(100);
                    break;
                case 2: // Obstacle Mode
                    Serial.println("Resuming WebSocketTask for Obstacle Mode, mode 2.");

                    ledControl.setMode(2);

                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) == eSuspended) {
                        self->webSocketTask.resumeTask();
                    }
                    
                    if (ultrasonicTask.getTaskHandle() != NULL) {
                        if (eTaskGetState(ultrasonicTask.getTaskHandle()) == eSuspended) {
                            Serial.println("from mode: resuming ultrasonic task");
                            ultrasonicTask.resumeTask();
                        } 
                    } else {
                            Serial.println("from mode: starting ultrasonic task");
                            ultrasonicTask.startTask();
                            ultrasonicMonitorTask.startTask();
                    }
                    buzzer.beep(200);
                    break;
                case 3:
                    Serial.println("Suspending WebSocketTask and starting auto follow, mode 3.");

                    ledControl.setMode(3);

                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    buzzer.beep(300);
                    break;

                case 4: // Auto Avoidance
                    Serial.println("Suspending WebSocketTask and starting auto avoidance, mode 4.");
                    
                    ledControl.setMode(4);

                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    buzzer.beep(400);
                    break;

                case 5: // Tuning Page
                    Serial.println("Suspending WebSocketTask and opening TUNING page), mode 5");

                    if (eTaskGetState(self->webSocketTask.getTaskHandle()) != eSuspended) {
                        self->webSocketTask.suspendTask();
                    }
                    buzzer.beep(500);  
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
            self->lastMode = mode;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}