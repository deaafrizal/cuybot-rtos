#include <Ultrasonic/UltrasonicTask.h>
#include <Arduino.h>

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorControl &motorControl, EEPROMConfig &eepromConfig)
    : _ultrasonic(ultrasonic), _motorControl(motorControl), taskRunning(false), _taskHandle(nullptr), _eepromConfig(eepromConfig) {
    _ultrasonic.begin();
}


UltrasonicTask::~UltrasonicTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
}

void UltrasonicTask::startTask() {
    if (_taskHandle == nullptr) {
        taskRunning = true;
        xTaskCreate(distanceMeasureTask, "UltrasonicTask", 3648, this, 2, &_taskHandle);
        if (_taskHandle != nullptr) {
            Serial.println("UltrasonicTask started successfully.");
            vTaskSuspend(_taskHandle);
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

    uint32_t lastSensorCheckTime = 0;
    uint32_t lastMotorUpdateTime = 0;
    const uint32_t sensorCheckInterval = 50;
    const uint32_t motorUpdateInterval = 5;

    while (true) {
        uint32_t currentTime = millis();

        if (self->taskRunning) {
            if (currentTime - lastSensorCheckTime >= sensorCheckInterval) {
                self->_distance = self->_ultrasonic.getDistance();
                lastSensorCheckTime = currentTime;
            }

            if (currentTime - lastMotorUpdateTime >= motorUpdateInterval) {
                uint8_t speed = self->_eepromConfig.getMemInt(1);

                if (self->_distance <= self->_maxDistance) {
                    self->_motorControl.setSpeed(255, 0);
                } else {
                    self->_motorControl.setSpeed(speed, speed);
                }
                lastMotorUpdateTime = currentTime;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}