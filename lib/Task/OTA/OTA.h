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
    const uint16_t _taskStackSize = 3072;
    const UBaseType_t _taskPriority = 4;
    const char* _hostname;
    TaskHandle_t _taskHandle;
};

#endif