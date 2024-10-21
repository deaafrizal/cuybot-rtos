#include <WebServer/WebServerTask.h>
#include <Routes/WebServerRoutes.h>
#include <SPIFFS.h>

AsyncWebServer WebServerTask::server = AsyncWebServer(80);
DNSServer WebServerTask::dnsServer;

WebServerTask::WebServerTask() : stackMonitorTask(nullptr) {
    Serial.println("WebServerTask initialized...");
    taskHandle = NULL;
}

WebServerTask::~WebServerTask() {
    Serial.println("WebServerTask cleanup...");
    stopTask();
    delete stackMonitorTask; // Clean up the StackMonitorTask instance
}

void WebServerTask::startTask(int stackSize) {
    Serial.println("Starting WebServer task...");
    if (taskHandle == NULL) {
        stackMonitorTask = new StackMonitorTask(taskHandle, stackSize, "WebServerTask"); // Create a new instance
        stackMonitorTask->startMonitoring(); // Start monitoring stack usage
        xTaskCreate(webServerTaskFunction, "WebServerTask", stackSize, this, 6, &taskHandle);
    }
}

void WebServerTask::stopTask() {
    Serial.println("Stopping WebServer task...");
    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
        stackMonitorTask->stopMonitoring(); // Stop monitoring stack usage
    }
}

void WebServerTask::webServerTaskFunction(void *parameter) {
    WebServerTask *self = static_cast<WebServerTask*>(parameter);

    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system");
        return;
    }

    WebServerRoutes routes(&server);
    Serial.println("Setting up webserver routes...");
    routes.setupRoutes();
    
    server.begin();
    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("Webserver & DNS activated!");

    unsigned long lastCheck = 0; // Variable to track the last check time

    for (;;) {
        dnsServer.processNextRequest();

        unsigned long currentMillis = millis();
        if (currentMillis - lastCheck >= 5000) { // Check every 5 seconds
            float usedStackPercentage = self->stackMonitorTask->getUsedStackPercentage();
            Serial.print("WebServer Task Used Stack Percentage: ");
            Serial.println(usedStackPercentage);
            lastCheck = currentMillis; // Update last check time
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Small delay to avoid busy-waiting
    }
}