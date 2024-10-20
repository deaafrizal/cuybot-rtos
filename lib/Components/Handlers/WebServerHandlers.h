#ifndef WEBSERVERHANDLERS_H
#define WEBSERVERHANDLERS_H
#include <EEPROM_config.h>
#include <ESPAsyncWebServer.h>

class WebServerHandlers {
public:
    WebServerHandlers(AsyncWebServer* server); 

    static void handleGetSystemData(AsyncWebServerRequest *request);
    static void handleSetMotorMaxSpeed(AsyncWebServerRequest *request);
    static void handleSetMotorWeight(AsyncWebServerRequest *request);
    static void handleNotFound(AsyncWebServerRequest *request);

private:
    AsyncWebServer* server;
    static EEPROMConfig &_eepromConfig;
};

extern int mode;

#endif