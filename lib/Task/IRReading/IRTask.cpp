#include <IRReading/IRTask.h> 

#define MAX_PWM_SPEED 255
#define MIN_PWM_SPEED 0

IRTask::IRTask(IR &ir, MotorDriver &rightMotor, MotorDriver &leftMotor) 
    : _ir(ir), _rightMotor(rightMotor), _leftMotor(leftMotor) {
    _taskHandle = nullptr;
    motorMaxSpeed = EEPROMConfig::getMemInt(1);
    motorWeight = EEPROMConfig::getMemInt(2);
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::startTask() {
    if (_taskHandle == nullptr) {
        xTaskCreate(irMeasureTask, "IRTask", 2448, this, 3, &_taskHandle);
        if (_taskHandle != nullptr) {
            Serial.println("IRTask started successfully.");
            vTaskSuspend(_taskHandle);
            Serial.println("IRTask initially suspended.");
        } else {
            Serial.println("Failed to start IRTask.");
        }
    }
}

void IRTask::stopTask() {
    if (_taskHandle != nullptr) {
        vTaskDelete(_taskHandle);
        _taskHandle = nullptr;
        _rightMotor.stop();
        _leftMotor.stop();
        Serial.println("IR Task stopped.");
    }
}

void IRTask::suspendTask() {
    if (_taskHandle != nullptr) {
        vTaskSuspend(_taskHandle);
        _rightMotor.stop();
        _leftMotor.stop();
        Serial.println("IR Task suspended.");
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != nullptr) {
        vTaskResume(_taskHandle);
        Serial.println("IR Task resumed.");
    }
}

void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);
    self->_ir.begin();
    
    while (true) {
        int irLeft = self->_ir.getIRLeft();
        int irRight = self->_ir.getIRRight();

        self->motorMaxSpeed = EEPROMConfig::getMemInt(1);
        self->motorWeight = EEPROMConfig::getMemInt(2);

        int leftMotorSpeed = self->motorMaxSpeed;
        int rightMotorSpeed = self->motorMaxSpeed;

        // Adjust motor speeds based on IR sensor values
        if (irLeft == LOW && irRight == HIGH) {
            leftMotorSpeed = self->motorMaxSpeed - self->motorWeight;
            rightMotorSpeed = self->motorMaxSpeed + self->motorWeight;
        } else if (irLeft == HIGH && irRight == LOW) {
            leftMotorSpeed = self->motorMaxSpeed + self->motorWeight;
            rightMotorSpeed = self->motorMaxSpeed - self->motorWeight;
        } else if (irLeft == LOW && irRight == LOW) {
            self->_rightMotor.stop();
            self->_leftMotor.stop();
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;  // Skip motor control and move to the next loop iteration
        } else {
            leftMotorSpeed = self->motorMaxSpeed;
            rightMotorSpeed = self->motorMaxSpeed;
        }

        self->_rightMotor.forward(self->motorMaxSpeed);
        self->_leftMotor.forward(self->motorMaxSpeed);
        // Delay between task executions
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // Stop motors when the task ends
    self->_rightMotor.stop();
    self->_leftMotor.stop();
    vTaskSuspend(nullptr);
}