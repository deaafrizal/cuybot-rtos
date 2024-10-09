#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <ArduinoOTA.h>

class OTA {
public:
    OTA(const char* hostname);
    void begin();
    void startOTATask();
    static void otaTask(void *parameter);

private:
    const char* _hostname;
    TaskHandle_t _taskHandle;
};

#endif