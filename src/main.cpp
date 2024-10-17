#include <EEPROM_config.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebServer/WebServerTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <Motor/MotorTask.h>
#include <OTA/OTA.h>
#include <IR/IR.h>
#include <IRReading/IRTask.h>
#include <Motor/MotorControl.h>
#include <Motor/MotorDriver.h>

EEPROMConfig eepromConfig;

#define PWM_A1 3
#define PWM_A2 4
#define PWM_B1 6
#define PWM_B2 5

#define SSID "cuybot"
const char* password = "123456789";

int mode = 0;
int motorSpeed = 0;
int motorDirection = 0;
uint8_t motorMaxSpeed = 60;
uint8_t motorWeight = 45;
float motorTurnFactor = 0.15;
bool userControllingDirection = false;

WebServerTask webServerTask;
OTA ota("cuybot");

MotorDriver leftMotor(PWM_B1, PWM_B2);
MotorDriver rightMotor(PWM_A1, PWM_A2);

MotorControl motorControl(rightMotor, leftMotor);
MotorTask motorTask(rightMotor, leftMotor);

IR ir;
IRTask irTask(ir, motorControl);

Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic, motorTask);

ModeSelectionTask modeSelectionTask(motorTask, ultrasonicTask, irTask);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting serial communication...");
    delay(1000);
    
    eepromConfig.loadSettings(motorMaxSpeed, motorWeight, motorTurnFactor);
    
    Serial.println("Serial communication OK!");
    Serial.println("Setting up WiFi...");
    
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "_");
    
    String ssid = String(SSID) + "_" + macAddr;

    if (WiFi.softAP(ssid.c_str(), password)) {
        Serial.println("Wi-Fi AP started successfully");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start Wi-Fi AP");
    }

    delay(1000);
    
    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }
    
    Serial.println("WiFi OK!");

    Serial.println("Setting up OTA service...");
    ota.begin();
    ota.startOTATask();
    Serial.println("Setting up OTA service done!");

    pinMode(PWM_A1, OUTPUT);
    pinMode(PWM_A2, OUTPUT);
    pinMode(PWM_B1, OUTPUT);
    pinMode(PWM_B2, OUTPUT);

    Serial.println("Setting up RTOS...");
    webServerTask.startTask();
    modeSelectionTask.startTask();
    motorTask.startTask();
    Serial.println("RTOS OK!");
}

void loop() {
    //
}

/* !Monitor The Stack Size
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(self->_taskHandle); // NULL instead if no pointer
    Serial.print("Initial free stack space (bytes): ");
    Serial.println(highWaterMark * sizeof(StackType_t));
*/