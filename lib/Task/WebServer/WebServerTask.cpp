#include <WebServer/WebServerTask.h>
#include <Routes/WebServerRoutes.h>
#include <SPIFFS.h>

AsyncWebServer WebServerTask::server = AsyncWebServer(80);
DNSServer WebServerTask::dnsServer;

WebServerTask::WebServerTask() {
    taskHandle = NULL;
}

WebServerTask::~WebServerTask() {
    stopTask();
}

void WebServerTask::startTask(int stackSize) {
    if (taskHandle == NULL) {
        xTaskCreate(webServerTaskFunction, "WebServerTask", stackSize, NULL, 1, &taskHandle);
    }
}

void WebServerTask::stopTask() {
    if (taskHandle != NULL) {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
    }
}

void WebServerTask::webServerTaskFunction(void *parameter) {
    if (!SPIFFS.begin(true)) {
        Serial.println("Failed to mount file system");
        return;
    }

    WebServerRoutes routes(&server);
    routes.setupRoutes();
    
    server.begin();
    dnsServer.start(53, "*", WiFi.softAPIP());

    for (;;) {
        dnsServer.processNextRequest();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}