#include <LineFollowing/LineFollowingTask.h>

LineFollowingTask::LineFollowingTask(IR &ir, MotorControl &motorControl) 
    : _ir(ir), _motorControl(motorControl), _taskHandle(NULL), _taskRunning(false) {}

void LineFollowingTask::startTask() {
    if (_taskHandle == NULL) {
        _taskRunning = true;
        xTaskCreate(irMeasureTask, "LineFollowingTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void LineFollowingTask::stopTask() {
    if (_taskHandle != NULL) {
        _motorControl.setSpeed(0, 0);
        _taskRunning = false;
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
        Serial.println("IR task stopped.");
    }
}

bool LineFollowingTask::getIsRunning() {
    return _taskRunning;
}

void LineFollowingTask::irMeasureTask(void *_parameters) {
    LineFollowingTask *self = static_cast<LineFollowingTask *>(_parameters);

    // atur nilai kp, ki, kd untuk penyesuaian track line followingnya
    // nilainya bebas, mulailah dari nilai 0.0 untuk ki & kd
    // kp bisa coba mulai dari nilai 1.0 untuk mengetahui perbedaan perilaku cuybotnya
    // nilai kp tinggi = belok akan patah (cocok untuk track extreme: zigzag, belokan tajam dan lain2)
    // nilai kp rendah = belok akan lebih smooth (cocok untuk track lingkaran)
    // naikin kp kalau dirasa ketika belok responnya kurang cepet
    // naikin ki & kd kalau kp udah pas (ciri-ciri kp pas adalah belok tidak terlalu patah & juga tidak terlalu slow response atau lambat)

    // adjust kp, ki & kd disini
    const float kp = 22;
    const float ki = 0.5;
    const float kd = 1;

    // adjust kecepatan awal & kecepatan paling lambat
    const int maxSpeed = 100;
    const int minSpeed = 26;
    int baseSpeed = minSpeed;
    
    // jangan ganti ini ya
    float lastError = 0;
    float integral = 0;
    static float prevDerivative = 0;

    while (self->_taskRunning) {
        int irLeft = self->_ir.getLeftSignal();
        int irRight = self->_ir.getRightSignal();
        int irMiddle = self->_ir.getMiddleSignal();

        int error = 0;

        if (irLeft == LOW && irMiddle == HIGH && irRight == LOW) {
            error = 0;
        } 
        else if (irLeft == HIGH && irMiddle == HIGH && irRight == LOW) {
            error = 1;
        } 
        else if (irLeft == LOW && irMiddle == HIGH && irRight == HIGH) {
            error = -1;
        }
        else if (irLeft == HIGH && irMiddle == LOW && irRight == LOW) {
            error = 2;
        } 
        else if (irLeft == LOW && irMiddle == LOW && irRight == HIGH) {
            error = -2;
        }

        float proportional = error * kp;
        integral += error * ki * (self->_vdelayTime / 1000.0);
        integral = constrain(integral, -minSpeed / 3, minSpeed / 3);
        
        if (error == 0) integral = 0;

        float rawDerivative = (error - lastError) / max(self->_vdelayTime / 1000.0, 0.01);
        float derivative = prevDerivative + 0.1 * (rawDerivative - prevDerivative); // Filter eksponensial
        prevDerivative = derivative;

        float pidOutput = proportional + integral + (kd * derivative);
        pidOutput = constrain(pidOutput, -maxSpeed, maxSpeed);

        if (abs(error) == 0) {
            baseSpeed = constrain(baseSpeed + 1, minSpeed, maxSpeed);
        } else if (abs(error) == 2) {
            baseSpeed = constrain(baseSpeed - (baseSpeed * 0.1), minSpeed, maxSpeed);
        } else {
            baseSpeed = constrain(baseSpeed - (baseSpeed * 0.02), minSpeed, maxSpeed);
        }

        int leftSpeed = baseSpeed - pidOutput;
        int rightSpeed = baseSpeed + pidOutput;

        leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);
        rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);

        self->_motorControl.setSpeed(rightSpeed, leftSpeed);

        lastError = error;

        vTaskDelay(pdMS_TO_TICKS(self->_vdelayTime));
    }

    Serial.println("IR task stopped.");
}