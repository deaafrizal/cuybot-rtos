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
#include <LedControl/LedControl.h>

// MOTOR PIN
#define PWM_A1_PIN 9
#define PWM_A2_PIN 10
#define PWM_B1_PIN 6
#define PWM_B2_PIN 7

// 4x microLED
#define LED_STATE_1_PIN 2
#define LED_STATE_2_PIN 8

// Ultrasonic
#define TRIGGER_PIN 20
#define ECHO_PIN 21

// Sound Buzzer
#define BUZZER_PIN 5

// IR Line Follow
#define IR_LEFT_PIN 1
#define IR_MIDDLE_PIM 3
#define IR_RIGHT_PIN 4

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
Buzzer buzzer(BUZZER_PIN);
LedControl ledControl(LED_STATE_1_PIN, LED_STATE_2_PIN);

IR ir(IR_LEFT_PIN, IR_RIGHT_PIN);

WebServerTask webServerTask;
WebSocketTask webSocketTask;

OTA ota("cuybot");

MotorDriver rightMotor(PWM_A1_PIN, PWM_A2_PIN);
MotorDriver leftMotor(PWM_B1_PIN, PWM_B2_PIN);

IRTask irTask(ir, rightMotor, leftMotor);

MotorControl motorControl(rightMotor, leftMotor);
MotorTask motorTask(rightMotor, leftMotor);

UltrasonicTask ultrasonicTask(ultrasonic);
ModeSelectionTask modeSelectionTask(ultrasonicTask, irTask, buzzer, ledControl);
HardwareMonitorTask hardwareMonitorTask(&webSocketTask);

BatteryMonitorTask batteryMonitorTask(BATTERY_ADC_PIN, VOLTAGE_MIN, VOLTAGE_MAX, VOLTAGE_DIVIDER_FACTOR, buzzer, &webSocketTask);

void setup() {
    Serial.begin(9600);
    Serial.println("Setting up WiFi...");
    
    delay(500);
    // WiFi.disconnect(true, true);
    String macAddr = WiFi.macAddress();
    String lastFourCharMacAddr = macAddr.substring(macAddr.length() - 4);
    String ssid = String(SSID) + "-" + lastFourCharMacAddr;

    if (WiFi.softAP(ssid, password)) {
        Serial.println("Wi-Fi AP started successfully");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    } else {
        Serial.println("Failed to start Wi-Fi AP");
    }

    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }

    delay(3000);
    Serial.println("Setting up OTA service...");
    ota.begin();
    ota.startOTATask();
    
    delay(500);
    EEPROM.begin(128);
    buzzer.begin();
    ultrasonic.begin();
    ir.begin();
    ledControl.begin();
    leftMotor.begin();
    rightMotor.begin();

    delay(300);
    Serial.println("Memory initializing...");
    eepromConfig.loadSettings();

    delay(300);
    Serial.println("RTOS initializing...");
    webServerTask.startTask();
    webSocketTask.startTask();
    modeSelectionTask.startTask();
    motorTask.startTask();
    hardwareMonitorTask.startTask();

    delay(400);
    Serial.println("...");
    batteryMonitorTask.startMonitoring();
    
    Serial.println("RTOS OK");
    Serial.println("open in browser http://cuybot.local");
}

void loop() {}
