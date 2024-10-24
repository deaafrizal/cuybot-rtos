#include <BatteryMonitor/BatteryMonitorTask.h>

BatteryMonitorTask::BatteryMonitorTask(uint8_t adcPin, float minVoltage, float maxVoltage, float voltageDividerFactor, Buzzer &buzzer, WebSocketTask* webSocketTask)
    : _adcPin(adcPin), _minVoltage(minVoltage), _maxVoltage(maxVoltage), 
      _voltageDividerFactor(voltageDividerFactor), _batteryVoltage(0), 
      _batteryPercentage(0), _highestVoltageDetected(0), 
      _taskHandle(NULL), _buzzer(buzzer), _webSocketTask(webSocketTask), 
      _lastSendTime(0) {}

void BatteryMonitorTask::startMonitoring() {
    if (_taskHandle == NULL) {
        xTaskCreate(monitorTask, "BatteryMonitorTask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void BatteryMonitorTask::stopMonitoring() {
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
    }
}

void BatteryMonitorTask::monitorTask(void *pvParameters) {
    BatteryMonitorTask *self = static_cast<BatteryMonitorTask *>(pvParameters);

    unsigned long startTime = millis();
    
    while (millis() - startTime < self->_calibrationTime) {
        self->readBattery(true);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    self->_highestVoltageDetected = self->_batteryVoltage;
    self->_calibrationComplete = true;
    self->_buzzer.playCalibrationBeep(self->_calibrationTime);

    while (true) {
        self->readBattery(false);
         if (self->_batteryPercentage <= 25) {
            self->playBatteryLowNotification();
        }
        self->sendBatteryData();
        vTaskDelay(pdMS_TO_TICKS(_MONITOR_INTERVAL));
    }
}

void BatteryMonitorTask::playBatteryLowNotification() {
    _buzzer.playBatteryLowWarning();
}

void BatteryMonitorTask::readBattery(bool duringCalibration) {
    int rawAdcValue = analogRead(_adcPin);

    float stepUpOutputVoltage = (rawAdcValue / 4095.0) * 3.3 * _voltageDividerFactor;
    float conversionFactor = 1.1385;
    float batteryVoltage = stepUpOutputVoltage / conversionFactor;

    if (duringCalibration) {
        if (batteryVoltage > _highestVoltageDetected) {
            _highestVoltageDetected = batteryVoltage;
        }
        _batteryVoltage = batteryVoltage;
    } else {
        if (batteryVoltage <= _highestVoltageDetected) {
            _batteryVoltage = batteryVoltage;
        }
    }

    float batteryPercentage = ((_batteryVoltage - _minVoltage) / (_maxVoltage - _minVoltage)) * 100.0;
    _batteryPercentage = constrain(batteryPercentage, 0, 100);
}

void BatteryMonitorTask::sendBatteryData() {
    unsigned long currentMillis = millis();
    if (currentMillis - _lastSendTime >= _DATA_SEND_INTERVAL) {
    String jsonData = "{\"batteryVoltage\": " + String(_batteryVoltage, 2) + 
                      ", \"batteryPercentage\": " + String(_batteryPercentage, 2) + "}";
        
        _webSocketTask->sendDataToClient(jsonData);
        _lastSendTime = currentMillis;
    }
}