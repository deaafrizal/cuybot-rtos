#include <WiFi.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <WebServer/WebServerTask.h>
#include <ESPmDNS.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define SSID "cuybot"
#define PASSWORD "123456789"

int mode = 0;

WebServerTask webServerTask;
ModeSelectionTask modeSelectionTask;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting serial communication...");
    delay(100);
    Serial.println("Serial communication OK!");

    Serial.println("Setting up WiFi...");
    WiFi.softAP(SSID, PASSWORD);
    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }
    Serial.println("WiFi OK!");

    Serial.println("Setting up RTOS...");
    webServerTask.startTask();
    modeSelectionTask.startTask();
    Serial.println("RTOS OK!");
    
    Serial.println("Setup complete. controller: http://cuybot.local ready!");

}


void loop() {}