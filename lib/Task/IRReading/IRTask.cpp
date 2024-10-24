#include <IRReading/IRTask.h> 

#define MAX_PWM_SPEED 255
#define MIN_PWM_SPEED 0

IRTask::IRTask(IR &ir, MotorDriver &rightMotor, MotorDriver &leftMotor) 
    : _ir(ir), _rightMotor(rightMotor), _leftMotor(leftMotor), _taskHandle(NULL), _taskRunning(false) {
    motorMaxSpeed = EEPROMConfig::getMemInt(1);
    motorWeight = EEPROMConfig::getMemInt(2);
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(irMeasureTask, "IRTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }else {
        _taskRunning = true; 
        Serial.println("IR task started.");
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        _rightMotor.stop();
        _leftMotor.stop();
        Serial.println("IR Task stopped.");
    }
}

void IRTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
        _rightMotor.stop();
        _leftMotor.stop();
        Serial.println("IR Task suspended.");
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle);
        Serial.println("IR Task resumed.");
    }
}

void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);
    self->_ir.begin();
    
    while (self->_taskRunning) {
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

    Serial.println("Ultrasonic taskrunning false.");
    self->_taskRunning = false;
}