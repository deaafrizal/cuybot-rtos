#include <MakeSound/MakeSoundTask.h>

MakeSound::MakeSound() : _buzzer(), _taskHandle(NULL) {
    _buzzer.begin();
}

void MakeSound::soundTask(void *_parameters) {
    MakeSound *self = static_cast<MakeSound *>(_parameters);

    while (true) {
        self->_buzzer.setState(true);
        self->_buzzer.beep(100);  // Make the buzzer beep for 500 ms
        vTaskDelay(pdMS_TO_TICKS(100));  // Wait for 2 seconds before the next beep
    }
}

void MakeSound::successSoundTask(void *_parameters) {
    MakeSound *self = static_cast<MakeSound *>(_parameters);

    while (true) {
        self->_buzzer.setState(true);
        self->_buzzer.beep(500);  // Make the buzzer beep for 500 ms
        vTaskDelay(pdMS_TO_TICKS(500));  // Wait for 2 seconds before the next beep
    }
}

void MakeSound::failedSoundTask(void *_parameters) {
    MakeSound *self = static_cast<MakeSound *>(_parameters);

    while (true) {
        self->_buzzer.setState(true);
        self->_buzzer.beep(100);  // Make the buzzer beep for 500 ms
        vTaskDelay(pdMS_TO_TICKS(100));  // Wait for 2 seconds before the next beep
    }
}

void MakeSound::startTask() {
  xTaskCreate(soundTask, "MakeSoundTask", 2048, this, 1, &_taskHandle);
}

void MakeSound::startSuccessSoundTask(int duration) {
    xTaskCreate(successSoundTask, "MakeSoundTask", 2048, this, 1, &_taskHandle);
    vTaskDelay(pdMS_TO_TICKS(duration));
    stopTask();
}

void MakeSound::startFailedSoundTask(int duration) {
    xTaskCreate(failedSoundTask, "MakeSoundTask", 2048, this, 1, &_taskHandle);
    vTaskDelay(pdMS_TO_TICKS(duration));
    stopTask();
}

void MakeSound::stopTask() {
    if (_taskHandle != NULL) {
        _buzzer.setState(false);
        _buzzer.off();
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

void MakeSound::suspendTask() {
    if (_taskHandle != NULL) {
        _buzzer.setState(false);
        vTaskSuspend(_taskHandle);
    }
}

void MakeSound::resumeTask() {
    if (_taskHandle != NULL) {
        _buzzer.setState(true);
        vTaskResume(_taskHandle);
    }
}