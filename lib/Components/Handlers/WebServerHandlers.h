#ifndef WEBSERVERHANDLERS_H
#define WEBSERVERHANDLERS_H

#include <ESPAsyncWebServer.h>

class WebServerHandlers {
public:
    WebServerHandlers(AsyncWebServer* server); 

    void setupRoutes();
    static void handleSetMaxSpeed(AsyncWebServerRequest *request);
    static void handleSetMode(AsyncWebServerRequest *request);
    static void handleNotFound(AsyncWebServerRequest *request);

private:
    AsyncWebServer* server;
};

extern int mode;

#endif