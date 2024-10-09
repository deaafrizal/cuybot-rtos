#include <WebServer/WebServerTask.h>
#include <Routes/WebServerRoutes.h>
#include <SPIFFS.h>

AsyncWebServer WebServerTask::server = AsyncWebServer(80);
DNSServer WebServerTask::dnsServer;

WebServerTask::WebServerTask() {
    Serial.println("webservertask initialize...");
    taskHandle = NULL;
}

WebServerTask::~WebServerTask() {
    Serial.println("webservertask cleanup...");
    stopTask();
}

void WebServerTask::startTask(int stackSize) {
    Serial.println("starting webserver task...");
    if (taskHandle == NULL) {
        xTaskCreate(webServerTaskFunction, "WebServerTask", stackSize, NULL, 7, &taskHandle);
    }
}

void WebServerTask::stopTask() {
    Serial.println("stoping webserver task...");
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
    Serial.println("setup webserver routes...");
    routes.setupRoutes();
    
    server.begin();
    dnsServer.start(53, "*", WiFi.softAPIP());
    Serial.println("webserver & DNS activated!");

    for (;;) {
        dnsServer.processNextRequest();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}