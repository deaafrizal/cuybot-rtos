#include <WiFi.h>
#include <ESPmDNS.h>
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebServer/WebServerTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <MakeSound/MakeSoundTask.h>
#include <Ultrasonic/UltrasonicTask.h>
#include <Ultrasonic/Ultrasonic.h>
#include <Ultrasonic/UltrasonicMonitorTask.h>

#define PWM_STANDBY 8
#define PWM_A1 4
#define PWM_A2 3
#define PWM_B1 5
#define PWM_B2 6

#define SSID "cuybot"
#define PASSWORD "123456789"

int mode = 1;

WebServerTask webServerTask;
ModeSelectionTask modeSelectionTask;
MakeSound makeSoundTask;
Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic);
UltrasonicMonitorTask ultrasonicMonitorTask(ultrasonic, ultrasonicTask);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting serial communication...");
    delay(100);

    ultrasonic.begin();

    pinMode(PWM_STANDBY, OUTPUT);
    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);

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

    // makeSoundTask.startSuccessSoundTask(3000);
    Serial.println("Setup complete. controller: http://cuybot.local ready!");
}

void loop() {
    //
}