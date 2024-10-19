#include <Ultrasonic/UltrasonicTask.h>
#include <Arduino.h>
#include <TelnetStream.h>
#include <EEPROM.h>

extern int motorSpeed;
extern int motorDirection;
extern bool userControllingDirection;

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl)
    : _ultrasonic(ultrasonic), _motorControl(motorControl), taskRunning(false), _taskHandle(nullptr) {
    _ultrasonic.begin();  // Initialize the ultrasonic sensor
}

void UltrasonicTask::startTask() {
    if (_taskHandle == nullptr) {
        taskRunning = true;  // Ensure taskRunning is true at start
        xTaskCreate(distanceMeasureTask, "UltrasonicTask", 3648, this, 2, &_taskHandle);
        if (_taskHandle != nullptr) {
            Serial.println("UltrasonicTask started successfully.");
            vTaskSuspend(_taskHandle);  // Start task in suspended state
            Serial.println("UltrasonicTask initially suspended.");
        } else {
            Serial.println("Failed to start UltrasonicTask.");
        }
    }
}

void UltrasonicTask::stopTask() {
    if (_taskHandle != nullptr) {
        taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("Ultrasonic task stopped.");
    }
}

TaskHandle_t UltrasonicTask::getTaskHandle() {
    return _taskHandle;
}

void UltrasonicTask::suspendTask() {
    if (_taskHandle != nullptr) {
        taskRunning = false;
        _motorControl.stop();
        vTaskSuspend(_taskHandle);
        Serial.println("Ultrasonic task suspended.");
    }
}

void UltrasonicTask::resumeTask() {
    if (_taskHandle != nullptr) {
        taskRunning = true;
        Serial.println("Resuming Ultrasonic task...");
        vTaskResume(_taskHandle);
        Serial.println("Ultrasonic task resumed.");
    }
}

void UltrasonicTask::distanceMeasureTask(void *parameters) {
    UltrasonicTask *self = static_cast<UltrasonicTask *>(parameters);

    while (true) {
        if (self->taskRunning) {
            self->_motorControl.setSpeed(EEPROM.read(1), EEPROM.read(1));
            self->_distance = self->_ultrasonic.getDistance();

            if(self->_distance >= 200) {
                self->_motorControl.stop();
            }else if (self->_distance > self->_minDistance && self->_distance <= self->_maxDistance) {
                self->_motorControl.setSpeedAndDirection(EEPROM.read(1), 1);
            }else {
                self->_motorControl.setSpeed(EEPROM.read(1), EEPROM.read(1));
            }

            TelnetStream.print("distance: ");
            TelnetStream.println(self->_distance);
        }
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
