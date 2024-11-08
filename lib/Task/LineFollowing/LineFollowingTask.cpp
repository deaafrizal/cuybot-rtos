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
    const float kp = 18.2;
    const float ki = 5.5;
    const float kd = 2.8;

    // adjust kecepatan awal & kecepatan paling lambat
    int normalSpeed = 100;              // 100 = 100% alias fullspeed, (range 0 - 100)
    int slowSpeed = normalSpeed / 2;    // 2x lebih lambat daripada kecepatan normal 
    
    // jangan ganti ini ya
    int baseSpeed = normalSpeed;
    float lastError = 0;
    float integral = 0;
    const int maxSpeed = 100;
    const int minSpeed = -100;

    while (self->_taskRunning) {
        int irLeft = self->_ir.getLeftSignal();
        int irRight = self->_ir.getRightSignal();
        int irMiddle = self->_ir.getMiddleSignal();

        int error = 0;

        if (irLeft == HIGH && irMiddle == HIGH && irRight == HIGH) {
            self->_motorControl.setSpeed(0, 0);
            continue;
        }

        if (irLeft == LOW && irMiddle == HIGH && irRight == LOW) {
            error = 0;
            baseSpeed = normalSpeed;
        } 
        else if (irLeft == HIGH && irMiddle == HIGH && irRight == LOW) {
            error = 1;
            baseSpeed = slowSpeed;
        } 
        else if (irLeft == LOW && irMiddle == HIGH && irRight == HIGH) {
            error = -1;
            baseSpeed = slowSpeed;
        }
        else if (irLeft == HIGH && irMiddle == LOW && irRight == LOW) {
            error = 2;
            baseSpeed = slowSpeed;
        } 
        else if (irLeft == LOW && irMiddle == LOW && irRight == HIGH) {
            error = -2;
            baseSpeed = slowSpeed;
        } else {
            baseSpeed = slowSpeed;
        }

        float proportional = error * kp;
        integral += error * ki * (self->_vdelayTime / 1000.0);
        integral = constrain(integral, -self->_vdelayTime * self->_vdelayTime, self->_vdelayTime * self->_vdelayTime);

        if (error == 0) integral = 0;

        float derivative = (error - lastError) * kd / (self->_vdelayTime / 1000.0);

        float pidOutput = proportional + integral + derivative;
        pidOutput = constrain(pidOutput, -baseSpeed, baseSpeed);

        int leftSpeed = baseSpeed - pidOutput;
        int rightSpeed = baseSpeed + pidOutput;

        leftSpeed = constrain(leftSpeed, minSpeed, maxSpeed);
        rightSpeed = constrain(rightSpeed, minSpeed, maxSpeed);

        self->_motorControl.setSpeed(rightSpeed, leftSpeed);

        lastError = error;

        vTaskDelay(pdMS_TO_TICKS(self->_vdelayTime));
    }

    Serial.println("IR task stopped.");
}