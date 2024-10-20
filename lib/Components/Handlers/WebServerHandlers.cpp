#include <Handlers/WebServerHandlers.h>

extern int mode;

WebServerHandlers::WebServerHandlers(AsyncWebServer* server) {
    this->server = server;
}

void WebServerHandlers::handleGetSystemData(AsyncWebServerRequest *request) {
    uint8_t lastMotorMaxSpeed = EEPROMConfig::getMemInt(1);
    uint8_t lastMotorWeight = EEPROMConfig::getMemInt(2);

    String jsonResponse = "{\"motorMaxSpeed\": " + String(lastMotorMaxSpeed) + 
                          ", \"motorWeight\": " + String(lastMotorWeight) +
                          ", \"mode\": " + String(mode) + "}";
    request->send(200, "application/json", jsonResponse);
}

void WebServerHandlers::handleSetMotorMaxSpeed(AsyncWebServerRequest *request) {
    if (request->hasParam("motorMaxSpeed", true)) {
        uint8_t newMaxSpeed = static_cast<uint8_t>(request->getParam("motorMaxSpeed", true)->value().toInt());
        Serial.printf("Received motorMaxSpeed: %d\n", newMaxSpeed);

        if (newMaxSpeed <= 255) {
            EEPROMConfig::motorMaxSpeed = newMaxSpeed;
            EEPROMConfig::setMemInt(1, newMaxSpeed);
            request->send(200, "application/json", "{\"status\":\"success\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid value\"}");
        }
    } else {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameter\"}");
    }
}

void WebServerHandlers::handleSetMotorWeight(AsyncWebServerRequest *request) {
    if (request->hasParam("motorWeight", true)) {
        uint8_t newMotorWeight = static_cast<uint8_t>(request->getParam("motorWeight", true)->value().toInt());
        Serial.printf("Received motorWeight: %d\n", newMotorWeight);

        if (newMotorWeight <= 255) {
            EEPROMConfig::motorWeight = newMotorWeight;
            EEPROMConfig::setMemInt(2, newMotorWeight);
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