#include <Handlers/WebServerHandlers.h>

WebServerHandlers::WebServerHandlers(AsyncWebServer* server) {
    this->server = server;
}

void WebServerHandlers::setupRoutes() {
    server->on("/api/setMaxSpeed", HTTP_POST, handleSetMaxSpeed);
    server->on("/api/setMode", HTTP_POST, handleSetMode);
    server->onNotFound(handleNotFound);
}

void WebServerHandlers::handleSetMaxSpeed(AsyncWebServerRequest *request) {
    if (request->hasParam("maxSpeed", true)) {
        int newSpeed = request->getParam("maxSpeed", true)->value().toInt();
        Serial.printf("Received maxSpeed: %d\n", newSpeed);
        if (newSpeed >= 0 && newSpeed <= 255) {
            request->send(200, "application/json", "{\"status\":\"success\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid value\"}");
        }
    } else {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameter\"}");
    }
}

void WebServerHandlers::handleSetMode(AsyncWebServerRequest *request) {
    if (request->hasParam("mode", true)) {
        int newMode = request->getParam("mode", true)->value().toInt();
        Serial.printf("Received mode: %d\n", newMode);
        if (newMode >= 0 && newMode <= 6) {
            if(mode != newMode) {
                mode = newMode;
            }
            request->send(200, "application/json", "{\"status\":\"success\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid value\"}");
        }
    } else {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameter\"}");
    }
}

void WebServerHandlers::handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/html", "Page not found");
}