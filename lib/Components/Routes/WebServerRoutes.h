#ifndef WEBSERVERROUTES_H
#define WEBSERVERROUTES_H

#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <Handlers/WebServerHandlers.h>

class WebServerRoutes {
public:
    WebServerRoutes(AsyncWebServer* server);
    void setupRoutes();

private:
    AsyncWebServer* server;

};

#endif