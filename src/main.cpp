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
#include <TelnetStream.h>
#include <EEPROM_config.h>

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

// ::MEM VAR:: extern to eeprom config
uint8_t motorMaxSpeed = 75;
uint8_t motorWeight = 50;
float motorTurnFactor = 0.2f;

WebServerTask webServerTask;
OTA ota("cuybot");

MotorDriver leftMotor(PWM_B1, PWM_B2);
MotorDriver rightMotor(PWM_A1, PWM_A2);

MotorControl motorControl(rightMotor, leftMotor, eepromConfig);
MotorTask motorTask(rightMotor, leftMotor, motorControl);

IR ir;
IRTask irTask(ir, motorControl, eepromConfig);

Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic, motorControl, eepromConfig);
ModeSelectionTask modeSelectionTask(motorTask, ultrasonicTask, irTask);
WebSocketTask webSocketTask(modeSelectionTask);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting serial communication...");
    delay(500);

    delay(200);

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
    TelnetStream.println("Setting up OTA service done!");

    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    
    delay(500);

    TelnetStream.println("Setting up RTOS...");
    webServerTask.startTask();
    modeSelectionTask.startTask();
    webSocketTask.startTask();
    motorTask.startTask();
    TelnetStream.println("RTOS OK!");
    Serial.println("RTOS OK!");
}

void loop() {
    //
}
