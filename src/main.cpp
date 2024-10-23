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
#include <HardwareMonitor/HardwareMonitorTask.h>

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
WebSocketTask webSocketTask;

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
HardwareMonitorTask hardwareMonitorTask(&webSocketTask);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting serial communication...");
    delay(10);
    EEPROM.begin(128);
    
    eepromConfig.loadSettings();

    Serial.println("Setting up WiFi...");
    
    String macAddr = WiFi.macAddress();
    
    String ssid = String(SSID) + "-" + macAddr;

    if (WiFi.softAP(ssid.c_str(), password, 6)) {
        Serial.println("Wi-Fi AP started successfully");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start Wi-Fi AP");
    }

    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }

    Serial.println("Setting up OTA service...");
    ota.begin();
    ota.startOTATask();

    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);
    
    delay(100);

    Serial.println("RTOS initialize...");
    webServerTask.startTask();
    webSocketTask.startTask();
    modeSelectionTask.startTask();
    motorTask.startTask();
    hardwareMonitorTask.startTask();
    
    Serial.println("RTOS OK");
    Serial.println("open in browser http://cuybot.local");
}

void loop() {}
