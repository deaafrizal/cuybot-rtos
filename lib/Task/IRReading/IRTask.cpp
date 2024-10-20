#include <IRReading/IRTask.h> 

#define MAX_PWM_SPEED 255
#define MIN_PWM_SPEED 0

IRTask::IRTask(IR &ir, MotorControl &motorControl, EEPROMConfig &eepromConfig) : _ir(ir), _motorControl(motorControl), _eepromConfig(eepromConfig) {
    _taskHandle = nullptr;
    _motorControl.stop();
    _motorMaxSpeed = _eepromConfig.getMemInt(1);
    _motorWeight = _eepromConfig.getMemInt(2);
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
        _motorControl.stop();
        Serial.println("IR Task stopped.");
    }
}

void IRTask::suspendTask() {
    if (_taskHandle != nullptr) {
        vTaskSuspend(_taskHandle);
        _motorControl.stop();
        Serial.println("IR Task suspended.");
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != nullptr) {
        _motorControl.setSpeed(_motorMaxSpeed, _motorMaxSpeed);
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

        self->_ir.printLog();

        int leftMotorSpeed = self->_motorMaxSpeed;
        int rightMotorSpeed = self->_motorMaxSpeed;

        if (irLeft == LOW && irRight == HIGH) {
            leftMotorSpeed = self->_motorMaxSpeed - self->_motorWeight;
            rightMotorSpeed = self->_motorMaxSpeed + self->_motorWeight;
        } else if (irLeft == HIGH && irRight == LOW) {
            leftMotorSpeed = self->_motorMaxSpeed + self->_motorWeight;
            rightMotorSpeed = self->_motorMaxSpeed - self->_motorWeight;
        }else if (irLeft == LOW && irRight == LOW) {
            self->_motorControl.stop();
        }

        leftMotorSpeed = constrain(leftMotorSpeed, MIN_PWM_SPEED, MAX_PWM_SPEED);
        rightMotorSpeed = constrain(rightMotorSpeed, MIN_PWM_SPEED, MAX_PWM_SPEED);

        self->_motorControl.setSpeed(leftMotorSpeed, rightMotorSpeed);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
    self->_motorControl.stop();
    vTaskSuspend(nullptr);
}