#include <OTA/OTA.h>

OTA::OTA(const char* hostname) : _hostname(hostname), _taskHandle(NULL) {}

void OTA::startOTATask() {
    if (_taskHandle == NULL) {
        xTaskCreate(otaTask, "OTATask", _taskStackSize, this, _taskPriority, &_taskHandle);
    }
}

void OTA::otaTask(void *parameter) {
    while (true) {
        ArduinoOTA.handle();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void OTA::begin() {
    ArduinoOTA.setHostname(_hostname);
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress * 100) / total);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
    Serial.println("OTA service ready!");
}
