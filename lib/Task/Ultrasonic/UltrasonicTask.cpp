#include <Ultrasonic/UltrasonicTask.h>
#include <Arduino.h>

UltrasonicTask::UltrasonicTask(Ultrasonic &ultrasonic, MotorDriver &rightMotor, MotorDriver &leftMotor)
    : _ultrasonic(ultrasonic), _rightMotor(rightMotor), _leftMotor(leftMotor), _taskHandle(nullptr) {
        ultrasonic.begin();
}

UltrasonicTask::~UltrasonicTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
    }
}

TaskHandle_t UltrasonicTask::getTaskHandle() {
    return _taskHandle;
}

void UltrasonicTask::startTask() {
    if (_taskHandle == nullptr) {
        xTaskCreate(distanceMeasureTask, "UltrasonicTask", 2748, this, 3, &_taskHandle);
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
        _rightMotor.stop();
        _leftMotor.stop();
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        Serial.println("Ultrasonic task stopped.");
    }
}

void UltrasonicTask::suspendTask() {
    if (_taskHandle != nullptr) {
        _rightMotor.stop();
        _leftMotor.stop();
        vTaskSuspend(_taskHandle);
        Serial.println("Ultrasonic task suspended.");
    }
}

void UltrasonicTask::resumeTask() {
    if (_taskHandle != nullptr) {
        motorMaxSpeed = EEPROMConfig::getMemInt(1);
        _rightMotor.forward(motorMaxSpeed);
        _leftMotor.forward(motorMaxSpeed);
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

    int previousLeftMotorSpeed = 0;
    int previousRightMotorSpeed = 0;

    while (true) {
        uint32_t currentTime = millis();

        if (currentTime - lastSensorCheckTime >= sensorCheckInterval) {
            self->_distance = self->_ultrasonic.getDistance();
            lastSensorCheckTime = currentTime;
        }

        if (currentTime - lastMotorUpdateTime >= motorUpdateInterval) {
            int leftMotorSpeed = self->motorMaxSpeed;
            int rightMotorSpeed = self->motorMaxSpeed;

            if (self->_distance <= self->_maxDistance) {
                rightMotorSpeed = -self->motorMaxSpeed;  // Reverse the right motor
            }

            // Only update motors if the speeds have changed
            if (leftMotorSpeed != previousLeftMotorSpeed || rightMotorSpeed != previousRightMotorSpeed) {
                // Apply motor speed changes
                if (leftMotorSpeed >= 0) {
                    self->_leftMotor.forward(leftMotorSpeed);
                } else {
                    self->_leftMotor.backward(-leftMotorSpeed);
                }

                if (rightMotorSpeed >= 0) {
                    self->_rightMotor.forward(rightMotorSpeed);
                } else {
                    self->_rightMotor.backward(-rightMotorSpeed);
                }

                // Save current motor speeds
                previousLeftMotorSpeed = leftMotorSpeed;
                previousRightMotorSpeed = rightMotorSpeed;
            }

            lastMotorUpdateTime = currentTime;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}