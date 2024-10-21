#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer/WebServerTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <Motor/MotorTask.h>
#include <WebSocket/WebSocketTask.h>
#include <OTA/OTA.h>
#include <IR/IR.h>
#include <IRReading/IRTask.h>
#include <Motor/MotorControl.h>
#include <Motor/MotorDriver.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <EEPROM_config.h>
#include <EEPROM.h>
#include <StackMonitor/StackMonitorTask.h>

TaskHandle_t webSocketTaskHandle = NULL;
const uint32_t webSocketStackSize = 4096;

TaskHandle_t webServerTaskHandle = NULL;
const uint32_t webServerStackSize = 2048;

#define PWM_A1 3
#define PWM_A2 4
#define PWM_B1 6
#define PWM_B2 5

#define SSID "cuybot"
const char* password = "123456789";

EEPROMConfig eepromConfig;

int mode = 1;
int motorSpeed = 0;
int motorDirection = 0;

WebServerTask webServerTask;
OTA ota("cuybot");

MotorDriver rightMotor(PWM_A1, PWM_A2);
MotorDriver leftMotor(PWM_B1, PWM_B2);

MotorControl motorControl(rightMotor, leftMotor);
MotorTask motorTask(rightMotor, leftMotor);

IR ir;
IRTask irTask(ir, rightMotor, leftMotor);

Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic, rightMotor, leftMotor);
ModeSelectionTask modeSelectionTask(motorTask, ultrasonicTask, irTask);
WebSocketTask webSocketTask;

StackMonitorTask webSocketTaskMonitoring(webSocketTaskHandle, webSocketStackSize, "webSocketTask", &webSocketTask);
StackMonitorTask webServerTaskMonitoring(webServerTaskHandle, webServerStackSize, "webServerTask", &webSocketTask);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting serial communication...");
    delay(500);
    EEPROM.begin(128);
    delay(1000);
    
    eepromConfig.loadSettings();

    Serial.println("Serial communication OK!");
    Serial.println("Setting up WiFi...");
    
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "_");
    
    String ssid = String(SSID) + "_" + macAddr;

    if (WiFi.softAP(ssid.c_str(), password, 6)) {
        Serial.println("Wi-Fi AP started successfully");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start Wi-Fi AP");
    }

    delay(500);
    
    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }
    
    Serial.println("WiFi OK!");

    Serial.println("Setting up OTA service...");
    ota.begin();
    ota.startOTATask();

    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    
    delay(500);

    Serial.println("RTOS initialize...");
    webServerTask.startTask(webServerTaskHandle, webServerStackSize);
    webSocketTask.startTask(webSocketTaskHandle, webSocketStackSize);
    
    modeSelectionTask.startTask();
    motorTask.startTask();
    
    delay(500);
    
    Serial.println("RTOS Start Monitoring...");
    webSocketTaskMonitoring.startMonitoring();
    webServerTaskMonitoring.startMonitoring();
    Serial.println("RTOS OK");
}

void loop() {}
