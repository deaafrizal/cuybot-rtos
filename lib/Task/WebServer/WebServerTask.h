#ifndef WEBSERVERTASK_H
#define WEBSERVERTASK_H

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

class WebServerTask {
public:
    WebServerTask();
    ~WebServerTask();
    
    void startTask(TaskHandle_t taskHandleint, uint32_t stackSize);
    void stopTask();

private:
    static void webServerTaskFunction(void *parameter);
    
    TaskHandle_t _taskHandle;
    static AsyncWebServer server;
    static DNSServer dnsServer; 
};

#endif