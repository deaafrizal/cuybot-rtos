#include <IRReading/IRTask.h>

extern int motorSpeed;
extern int motorDirection; 

IRTask::IRTask(IR &ir, MotorDriver &rightMotor, MotorDriver &leftMotor) 
    : _ir(ir), _rightMotor(rightMotor), _leftMotor(leftMotor), _taskHandle(NULL), _taskRunning(false) {
    // motorMaxSpeed = EEPROMConfig::getMemInt(1);
    // motorWeight = EEPROMConfig::getMemInt(2);
    // motorSpeed = motorMaxSpeed;
}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(irMeasureTask, "IRTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
        motorSpeed = 0;
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("IR task stopped.");
    }
}

bool IRTask::getIsRunning() {
    return _taskRunning;
}


void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);

    while (self->_taskRunning) {
        int irLeft = self->_ir.getLeftSignal();
        int irRight = self->_ir.getRightSignal();

        motorSpeed = 100;

        if (irLeft == LOW && irRight == HIGH) {
           motorDirection = 0;
        } else if(irLeft == HIGH && irRight == LOW) {
            motorDirection = 1;
        } else if(irLeft == HIGH && irRight == HIGH) {
            motorDirection = -1;
        }else {
            motorSpeed = 0;
            motorDirection = 0;
        }

        // self->motorMaxSpeed = EEPROMConfig::getMemInt(1);
        // self->motorWeight = EEPROMConfig::getMemInt(2);

        // int leftMotorSpeed = self->motorMaxSpeed;
        // int rightMotorSpeed = self->motorMaxSpeed;

        // if (irLeft == LOW && irRight == HIGH) {
        //     leftMotorSpeed = self->motorMaxSpeed - self->motorWeight;
        //     rightMotorSpeed = self->motorMaxSpeed + self->motorWeight;
        // } else if (irLeft == HIGH && irRight == LOW) {
        //     leftMotorSpeed = self->motorMaxSpeed + self->motorWeight;
        //     rightMotorSpeed = self->motorMaxSpeed - self->motorWeight;
        // } else if (irLeft == LOW && irRight == LOW) {
        //     self->_rightMotor.stop();
        //     self->_leftMotor.stop();
        //     vTaskDelay(pdMS_TO_TICKS(10));
        //     continue;
        // } else {
        //     leftMotorSpeed = self->motorMaxSpeed;
        //     rightMotorSpeed = self->motorMaxSpeed;
        // }
        // self->_rightMotor.forward(self->motorMaxSpeed);
        // self->_leftMotor.forward(self->motorMaxSpeed);
       

        vTaskDelay(pdMS_TO_TICKS(50));
    }

    Serial.println("IR taskrunning false.");
}