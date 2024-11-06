#include <IRReading/IRTask.h>

IRTask::IRTask(IR &ir, MotorControl &motorControl) 
    : _ir(ir), _motorControl(motorControl), _taskHandle(NULL), _taskRunning(false) {}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(irMeasureTask, "IRTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
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

    // setting value nya jika di perlukan, sesuaikan dengan kondisi track.
    // range kecepatan (0-100%). value di convert ke pwm pada function setSpeed motorControl.cpp
    const int normalSpeed = 65; // speed = 65%
    const int slowSpeed = 20; //speed = 20%, 
    int baseSpeed = normalSpeed;

    while (self->_taskRunning) {
        int irLeft = self->_ir.getLeftSignal();
        int irRight = self->_ir.getRightSignal();

        if (irLeft == LOW && irRight == LOW) { // garis di IR kanan
            self->_motorControl.setSpeed(normalSpeed, normalSpeed);
            baseSpeed = normalSpeed;
        } 
        else if (irLeft == HIGH && irRight == LOW) { // belok kanan
            self->_motorControl.setSpeed(normalSpeed, slowSpeed);
            baseSpeed = slowSpeed;
        } 
        else if (irLeft == LOW && irRight == HIGH) { // garis di IR kiri
            self->_motorControl.setSpeed(slowSpeed, normalSpeed); // belok kiri
            baseSpeed = slowSpeed;
        } 
        else {
            self->_motorControl.stop();
        }

        vTaskDelay(pdMS_TO_TICKS(self->_vdelayTime));
    }

    Serial.println("IR task stopped.");
}