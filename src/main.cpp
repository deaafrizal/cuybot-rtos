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
#include <Buzzer/Buzzer.h>
#include <BatteryMonitor/BatteryMonitorTask.h>

// MOTOR PIN
#define PWM_A1 3
#define PWM_A2 4
#define PWM_B1 6
#define PWM_B2 5

// Ultrasonic
#define TRIGGER_PIN 20
#define ECHO_PIN 21
// BAT CALC
#define BATTERY_ADC_PIN 0
#define VOLTAGE_DIVIDER_FACTOR 2
const float VOLTAGE_MIN = 2.8;
const float VOLTAGE_MAX = 4.2;
// WIFI CONF
#define SSID "cuybot"
const char* password = "cuybot123";
// EXTERN VAR
int mode = 1;
int motorSpeed = 0;
int motorDirection = 0;

EEPROMConfig eepromConfig;
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
Buzzer buzzer(1);
IR ir;

WebServerTask webServerTask;
WebSocketTask webSocketTask;

OTA ota("cuybot");

MotorDriver rightMotor(PWM_A1, PWM_A2);
MotorDriver leftMotor(PWM_B1, PWM_B2);
IRTask irTask(ir, rightMotor, leftMotor);

MotorControl motorControl(rightMotor, leftMotor);
MotorTask motorTask(rightMotor, leftMotor);

UltrasonicTask ultrasonicTask(ultrasonic);
ModeSelectionTask modeSelectionTask(ultrasonicTask, irTask, buzzer);
HardwareMonitorTask hardwareMonitorTask(&webSocketTask);

BatteryMonitorTask batteryMonitorTask(BATTERY_ADC_PIN, VOLTAGE_MIN, VOLTAGE_MAX, VOLTAGE_DIVIDER_FACTOR, buzzer, &webSocketTask);

void setup() {
    Serial.begin(9600);
    Serial.println("Starting serial communication...");
    WiFi.disconnect(true, true);
    delay(500);
    
    EEPROM.begin(128);
    eepromConfig.loadSettings();
    
    buzzer.begin();
    ultrasonic.begin();
    
    delay(1000);
    Serial.println("Setting up WiFi...");


    String macAddr = WiFi.macAddress();
    String lastFourCharMacAddr = macAddr.substring(macAddr.length() - 4);
    String ssid = String(SSID) + "-" + lastFourCharMacAddr;

    if (WiFi.softAP(ssid, password, 6)) {
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
    
    delay(100);

    Serial.println("RTOS initialize...");
    webServerTask.startTask();
    webSocketTask.startTask();
    modeSelectionTask.startTask();
    motorTask.startTask();
    hardwareMonitorTask.startTask();
    batteryMonitorTask.startMonitoring();
    
    Serial.println("RTOS OK");
    Serial.println("open in browser http://cuybot.local");
}

void loop() {}
