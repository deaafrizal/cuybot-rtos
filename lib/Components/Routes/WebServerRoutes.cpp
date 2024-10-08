#include <Arduino.h>
#include <Routes/WebServerRoutes.h>
#include <Handlers/WebServerHandlers.h>

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
        mode = 4;
        request->send(SPIFFS, "/html/tuning.html", "text/html");
    });

    server->on("/flash", HTTP_GET, [](AsyncWebServerRequest *request) {
        mode = 5;
        Serial.println("Flash Mode Activated!");
        request->send(SPIFFS, "/html/flash.html", "text/html");
    });

    server->on("/api/setMaxSpeed", HTTP_POST, WebServerHandlers::handleSetMaxSpeed);

    server->on("/api/setMode", HTTP_POST, WebServerHandlers::handleSetMode);

    server->onNotFound(WebServerHandlers::handleNotFound);
}