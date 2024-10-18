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

    server->on("/tuning", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Tuning Mode Activated!");
        request->send(SPIFFS, "/html/tuning.html", "text/html");
    });

    server->on("/flash", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.println("Flash Mode Activated!");
        request->send(SPIFFS, "/html/flash.html", "text/html");
    });

    server->on("/api/getSystemData", HTTP_GET, WebServerHandlers::handleGetSystemData);
    server->on("/api/setMotorMaxSpeed", HTTP_POST, WebServerHandlers::handleSetMotorMaxSpeed);
    server->on("/api/setMotorWeight", HTTP_POST, WebServerHandlers::handleSetMotorWeight);

    server->onNotFound(WebServerHandlers::handleNotFound);
}