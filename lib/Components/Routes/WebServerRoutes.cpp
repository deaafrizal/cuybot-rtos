#include <Arduino.h>
#include <Routes/WebServerRoutes.h>
#include <Handlers/WebServerHandlers.h>

extern int mode;

WebServerRoutes::WebServerRoutes(AsyncWebServer* server) {
    this->server = server;
}

void WebServerRoutes::setupRoutes() {
    server->serveStatic("/", SPIFFS, "/");

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        mode = 1;
        Serial.println("Mode set to 1 (Manual Control)");
        request->send(SPIFFS, "/html/index.html", "text/html");
    });
}