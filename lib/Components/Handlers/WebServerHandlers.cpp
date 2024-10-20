#include <Handlers/WebServerHandlers.h>

EEPROMConfig WebServerHandlers::_eepromConfig;

extern int mode;

WebServerHandlers::WebServerHandlers(AsyncWebServer* server) {
    this->server = server;
}
// Handle GET request to fetch motorMaxSpeed, motorWeight, and mode
void WebServerHandlers::handleGetSystemData(AsyncWebServerRequest *request) {
    int lastMotorMaxSpeed = _eepromConfig.getMemInt(1);  // Use global eepromConfig
    int lastMotorWeight = _eepromConfig.getMemInt(2);
    
    String jsonResponse = "{\"motorMaxSpeed\": " + String(lastMotorMaxSpeed) + 
                        ", \"motorWeight\": " + String(lastMotorWeight) +
                        ", \"mode\": " + String(mode) + "}";
    request->send(200, "application/json", jsonResponse);
}

// Handle POST request to set motorMaxSpeed (PWM value 0-255)
void WebServerHandlers::handleSetMotorMaxSpeed(AsyncWebServerRequest *request) {
    if (request->hasParam("motorMaxSpeed", true)) {
        uint8_t newMaxSpeed = static_cast<uint8_t>(request->getParam("motorMaxSpeed", true)->value().toInt());
        Serial.printf("Received motorMaxSpeed: %d\n", newMaxSpeed);

        // Since newMaxSpeed is already uint8_t, we can skip the cast and just assign
        if (newMaxSpeed <= 255) {  // No need to check >= 0, since uint8_t cannot be negative
            _eepromConfig.setMemInt(1, newMaxSpeed);
            request->send(200, "application/json", "{\"status\":\"success\"}");
        } else {
            request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid value\"}");
        }
    } else {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Missing parameter\"}");
    }
}

// Handle POST request to set motorWeight (PWM value 0-255)
void WebServerHandlers::handleSetMotorWeight(AsyncWebServerRequest *request) {
    if (request->hasParam("motorWeight", true)) {
        uint8_t newMotorWeight = static_cast<uint8_t>(request->getParam("motorWeight", true)->value().toInt());
        Serial.printf("Received motorWeight: %d\n", newMotorWeight);

        if (newMotorWeight <= 255) {  // No need to check >= 0 since uint8_t cannot be negative
            _eepromConfig.setMemInt(2, newMotorWeight);

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