#include <WiFi.h>
#include <ESPmDNS.h>
#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <WebServer/WebServerTask.h>
#include <ModeSelection/ModeSelectionTask.h>
#include <Motor/MotorTask.h>
#include <OTA/OTA.h>

#define PWM_A1 3
#define PWM_A2 4
#define PWM_B1 6
#define PWM_B2 5

#define SSID "cuybot"
#define PASSWORD "123456789"

int mode = 1;
int speed = 0;
int direction = 0;
bool userControllingDirection = false;

WebServerTask webServerTask;
OTA ota("cuybot");
MotorTask motorTask(PWM_A1, PWM_A2, PWM_B1, PWM_B2);
Ultrasonic ultrasonic;
UltrasonicTask ultrasonicTask(ultrasonic, motorTask);
ModeSelectionTask modeSelectionTask(motorTask, ultrasonicTask);

void setup() {
    Serial.begin(115200);
    Serial.println("Starting serial communication...");
    delay(1000);

    Serial.println("Serial communication OK!");
    Serial.println("Setting up WiFi...");
    WiFi.softAP(SSID, PASSWORD);
    if (!MDNS.begin("cuybot")) {
        Serial.println("DNS Cannot be started!");
    }
    
    delay(2000);
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

    Serial.println("Setup complete. controller: http://cuybot.local ready!");
    esp_reset_reason_t reason = esp_reset_reason();
    Serial.print("Reset reason: ");
    Serial.println(reason);
}

void loop() {
//
}

/* !Monitor The Stack Size
    UBaseType_t highWaterMark = uxTaskGetStackHighWaterMark(self->_taskHandle); // NULL instead if no pointer
    Serial.print("Initial free stack space (bytes): ");
    Serial.println(highWaterMark * sizeof(StackType_t));
*/