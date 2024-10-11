#include <IRReading/IRTask.h> 

IRTask::IRTask(IR &ir, MotorTask &motorTask) : _ir(ir), _motorTask(motorTask) {
    taskRunning = false;
    _taskHandle = NULL;
}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(
            irMeasureTask, 
            "IR Task", 
            2048, 
            this, 
            5, 
            &_taskHandle
        );
    }
    if (_taskHandle != NULL) {
            taskRunning = true;
            Serial.println("IR task started.");
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
        Serial.println("IR task stopped!");
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        taskRunning = false;
    }
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::suspendTask() {
    if (_taskHandle != NULL) {
        Serial.println("IR task suspend!");
        vTaskSuspend(_taskHandle);
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != NULL) {
        Serial.println("IR task resume!");
        vTaskResume(_taskHandle);
    }
}

void IRTask::irMeasureTask(void *_parameters) {
    IRTask *self = static_cast<IRTask *>(_parameters);
    self->_ir.begin();

    while (self->taskRunning) {
        self->_ir.printLog();
        
        if (self->_ir.getIRLeft() == LOW && self->_ir.getIRRight() == LOW) {
            Serial.println("Both IR sensors active, stopping.");
        }

        else if (self->_ir.getIRLeft() == LOW) {
            Serial.println("Left IR sensor active, turning right.");
            self->_motorTask.setDirection(1);
        }

        else if (self->_ir.getIRRight() == LOW) {
            Serial.println("Right IR sensor active, turning left.");
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    self->_taskHandle = NULL;
    vTaskDelete(NULL);  
}
