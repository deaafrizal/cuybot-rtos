#include <ModeSelection/ModeSelectionTask.h>
#include <LedControl/LedControl.h>
#include <Buzzer/Buzzer.h>

LedControl ledControl;
Buzzer buzzer(1);
extern int mode;

ModeSelectionTask::ModeSelectionTask(MotorTask &motor, UltrasonicTask &ultrasonicTask)
    : _motorTask(motor), _ultrasonicTask(ultrasonicTask), lastMode(-1){
        Serial.println("initializing modeselection...");
        taskHandle = NULL;
        buzzer.begin();
    }

ModeSelectionTask::~ModeSelectionTask() {
    Serial.println("ModeSelection Task cleanup...");
    stopTask();
}

void ModeSelectionTask::startTask(int stackSize) {
    Serial.println("ModeSelection Task starting up...");
    if (taskHandle == NULL) {
        xTaskCreate(modeSelectionTaskFunction, "ModeSelectionTask", stackSize, this, 6, &taskHandle);
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

                    if (self->_ultrasonicTask.getTaskHandle() != NULL) {
                        if (eTaskGetState(self->_ultrasonicTask.getTaskHandle()) != eSuspended) {
                            self->_ultrasonicTask.stopTask();
                            Serial.println("UltrasonicTask and");
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
                    
                    if (self->_ultrasonicTask.getTaskHandle() != NULL) {
                        if (eTaskGetState(self->_ultrasonicTask.getTaskHandle()) == eSuspended) {
                            Serial.println("from mode: resuming ultrasonic task");
                            self->_ultrasonicTask.resumeTask();
                        } 
                    }else {
                        Serial.println("from mode: starting ultrasonic task");
                        self->_ultrasonicTask.startTask();
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