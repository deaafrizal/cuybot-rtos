#include <WebServer/WebServerTask.h>
#include <Routes/WebServerRoutes.h>
#include <SPIFFS.h>

AsyncWebServer WebServerTask::server = AsyncWebServer(80);
DNSServer WebServerTask::dnsServer;
SemaphoreHandle_t WebServerTask::dnsSemaphore = NULL;

WebServerTask::WebServerTask() {
    Serial.println("WebServerTask initialized...");
    _taskHandle = NULL;
    dnsSemaphore = xSemaphoreCreateBinary();
}

WebServerTask::~WebServerTask() {
    Serial.println("WebServerTask cleanup...");
    stopTask();
    if (dnsSemaphore != NULL) {
        vSemaphoreDelete(dnsSemaphore);
        dnsSemaphore = NULL;
    }
}

void WebServerTask::startTask() {
    Serial.println("Starting WebServer task...");
    if (_taskHandle == NULL) {
        xTaskCreate(webServerTaskFunction, "WebServerTask", _taskStackSize, this, _taskPriority, &_taskHandle);
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
        if (xSemaphoreTake(self->dnsSemaphore, portMAX_DELAY) == pdTRUE) {
            dnsServer.processNextRequest();
        }
    }
}

void WebServerTask::triggerDNSServer() {
    xSemaphoreGive(dnsSemaphore);
}