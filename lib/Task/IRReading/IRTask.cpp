#include <IRReading/IRTask.h> 

extern uint8_t motorMaxSpeed;
extern uint8_t motorWeight;

IRTask::IRTask(IR &ir, MotorControl &motorControl) : _ir(ir), _motorControl(motorControl) {
    _taskHandle = nullptr;
    _motorControl.stop();
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::startTask() {
    if (_taskHandle == nullptr) {
        xTaskCreate(irMeasureTask, "IRTask", 3048, this, 5, &_taskHandle);
        if (_taskHandle != nullptr) {
            Serial.println("IRTask started successfully.");
            vTaskSuspend(_taskHandle);  // Start task in suspended state
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
        _motorControl.setSpeed(motorMaxSpeed, motorMaxSpeed);  // Set initial motor speed
        vTaskResume(_taskHandle);
        Serial.println("IR Task resumed.");
    }
}

void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);

    self->_ir.begin();
    int maxPWMSpeed = 255;
    int minSpeed = 0;
    
    while (true) {  // Keep the task running indefinitely
        int baselineSpeed = motorMaxSpeed;

        int irLeft = self->_ir.getIRLeft();
        int irRight = self->_ir.getIRRight();

        self->_ir.printLog();

        int leftMotorSpeed = baselineSpeed;
        int rightMotorSpeed = baselineSpeed;

        // Adjust motor speeds based on IR sensor readings
        if (irLeft == LOW && irRight == HIGH) {
            leftMotorSpeed = baselineSpeed - motorWeight;
            rightMotorSpeed = baselineSpeed + motorWeight;
        } else if (irLeft == HIGH && irRight == LOW) {
            leftMotorSpeed = baselineSpeed + motorWeight;
            rightMotorSpeed = baselineSpeed - motorWeight;
        }else if (irLeft == LOW && irRight == LOW) {
            self->_motorControl.stop();
        }

        // Constrain motor speeds
        leftMotorSpeed = constrain(leftMotorSpeed, minSpeed, maxPWMSpeed);
        rightMotorSpeed = constrain(rightMotorSpeed, minSpeed, maxPWMSpeed);

        // Set motor speeds
        self->_motorControl.setSpeed(leftMotorSpeed, rightMotorSpeed);

        vTaskDelay(pdMS_TO_TICKS(10));  // Adjust delay as needed
    }
    self->_motorControl.stop();
}