#include <IRReading/IRTask.h> 

IRTask::IRTask(IR &irSensor) : _ir(irSensor), taskRunning(false), _taskHandle(NULL) {}

void IRTask::irMeasureTask(void *_parameters) {
    IRTask *task = static_cast<IRTask *>(_parameters);  // Cast parameters to IRTask object
    task->taskRunning = true;

    while (task->taskRunning) {
        task->_ir.avoidance();
        task->_ir.printLog();
        vTaskDelay(task->_vdelayTime / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void IRTask::startTask() {
    if (_taskHandle == NULL) {
        xTaskCreate(irMeasureTask, "IR Task", 2048, this, 4, &_taskHandle);
    }
}

void IRTask::stopTask() {
    if (_taskHandle != NULL) {
        _taskHandle = NULL;
        taskRunning = false;
    }
}

TaskHandle_t IRTask::getTaskHandle() {
    return _taskHandle;
}

void IRTask::suspendTask() {
    if (_taskHandle != NULL) {
        vTaskSuspend(_taskHandle);
    }
}

void IRTask::resumeTask() {
    if (_taskHandle != NULL) {
        vTaskResume(_taskHandle);
    }
}