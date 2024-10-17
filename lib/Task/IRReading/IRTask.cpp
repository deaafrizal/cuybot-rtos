#include <IRReading/IRTask.h> 

extern uint8_t motorMaxSpeed;
extern uint8_t motorWeight;

IRTask::IRTask(IR &ir, MotorControl &motorControl) : _ir(ir), _motorControl(motorControl) {
    taskRunning = false;
    _taskHandle = NULL;
    _motorControl.stop();
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        taskRunning = true;
        xTaskCreate(
            irMeasureTask, 
            "IR Task", 
            3048, 
            this, 
            5, 
            &_taskHandle
        );
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
        taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        _motorControl.stop();
        Serial.println("IR task stopped!");
    }
}

void IRTask::suspendTask() {
    if (_taskHandle != NULL) {
        taskRunning = false;
        vTaskSuspend(_taskHandle);
        _motorControl.stop();
        Serial.println("IR task suspend!");
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != NULL) {
        taskRunning = true;
        _motorControl.setSpeed(motorMaxSpeed, motorMaxSpeed);
        vTaskResume(_taskHandle);
        Serial.println("IR task resume!");
    }
}


void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);

    self->_ir.begin();
    int maxPWMSpeed = 255;
    int minSpeed = 0;
    
    while (self->taskRunning) {
        int baselineSpeed = motorMaxSpeed;

        int irLeft = self->_ir.getIRLeft();
        int irRight = self->_ir.getIRRight();

        int leftMotorSpeed = baselineSpeed;
        int rightMotorSpeed = baselineSpeed;

        if (irLeft == LOW && irRight == HIGH) {
            leftMotorSpeed = baselineSpeed - motorWeight;
            rightMotorSpeed = baselineSpeed + motorWeight;
        } else if (irLeft == HIGH && irRight == LOW) {
            leftMotorSpeed = baselineSpeed + motorWeight;
            rightMotorSpeed = baselineSpeed - motorWeight;
        }

        leftMotorSpeed = constrain(leftMotorSpeed, minSpeed, maxPWMSpeed);
        rightMotorSpeed = constrain(rightMotorSpeed, minSpeed, maxPWMSpeed);

        self->_motorControl.setSpeed(leftMotorSpeed, rightMotorSpeed);

        vTaskDelay(pdMS_TO_TICKS(5));
    }
    self->_motorControl.stop();
}