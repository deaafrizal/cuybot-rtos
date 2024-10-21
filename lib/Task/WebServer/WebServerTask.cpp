#include <WebServer/WebServerTask.h>
#include <Routes/WebServerRoutes.h>
#include <SPIFFS.h>

AsyncWebServer WebServerTask::server = AsyncWebServer(80);
DNSServer WebServerTask::dnsServer;

WebServerTask::WebServerTask() {
    Serial.println("WebServerTask initialized...");
    _taskHandle = NULL;
}

WebServerTask::~WebServerTask() {
    Serial.println("WebServerTask cleanup...");
    stopTask();
}

void WebServerTask::startTask(TaskHandle_t taskHandle, uint32_t stackSize) {
    Serial.println("Starting WebServer task...");
    if (_taskHandle == NULL) {
        _taskHandle = taskHandle;

        xTaskCreate(webServerTaskFunction, "WebServerTask", stackSize, this, 6, &_taskHandle);
    }
}

void WebServerTask::stopTask() {
    Serial.println("Stopping WebServer task...");
    if (_taskHandle != NULL) {
        vTaskDelete(_taskHandle);
        _taskHandle = NULL;
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

    for (;;) {
        dnsServer.processNextRequest();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}