#ifndef WEBSERVERTASK_H
#define WEBSERVERTASK_H

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>

class WebServerTask {
public:
    WebServerTask();
    ~WebServerTask();
    
    void startTask(int stackSize = 2176);
    void stopTask();

private:
    static void webServerTaskFunction(void *parameter);

    TaskHandle_t taskHandle;
    static AsyncWebServer server;
    static DNSServer dnsServer; 
};

#endif