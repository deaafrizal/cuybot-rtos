#include <OTA/OTA.h>
#include <TelnetStream.h>

void handleTelnetInput() {
    String command = "";  // Buffer for the input string
    int incomingByte;

    // Read characters from the TelnetStream until a newline or no input
    while ((incomingByte = TelnetStream.read()) > 0) {
        // If newline is detected, stop reading
        if (incomingByte == '\n' || incomingByte == '\r') {
            break;
        }
        // Append each character to the command buffer
        command += (char)incomingByte;
    }

    // Check if the command is 'exit' or 'quit'
    command.trim();  // Remove any extra spaces or newline characters
    if (command == "x" || command == "q") {
        TelnetStream.println("Closing Telnet session, goodbye!");
        vTaskDelay(pdMS_TO_TICKS(100));
        TelnetStream.stop();  // Close the Telnet connection
    } else if (command.length() > 0) {
        // Handle other commands if needed
        TelnetStream.println("Unknown command: " + command);
    }
}


OTA::OTA(const char* hostname) : _hostname(hostname), _taskHandle(NULL) {}

void OTA::begin() {
    ArduinoOTA.setHostname(_hostname);
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else {
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
        TelnetStream.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
        TelnetStream.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress * 100) / total);
         TelnetStream.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        TelnetStream.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
    Serial.println("OTA service ready!");
    TelnetStream.begin();
    TelnetStream.println("Telnet ready. Use 'telnet " + WiFi.localIP().toString() + " 23' to connect.");
}

void OTA::otaTask(void *parameter) {
    while (true) {
        ArduinoOTA.handle();
        handleTelnetInput();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void OTA::startOTATask() {
    if (_taskHandle == NULL) {
        xTaskCreate(otaTask, "OTATask", 2048, this, 3, &_taskHandle);
    }
}