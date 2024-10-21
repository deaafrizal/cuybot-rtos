#ifndef WEBSERVERTASK_H
#define WEBSERVERTASK_H

#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <StackMonitor/StackMonitorTask.h>

class WebServerTask {
public:
    WebServerTask();
    ~WebServerTask();
    
    void startTask(int stackSize = 6276);
    void stopTask();
    void monitorStack(unsigned long currentMillis); // For monitoring stack usage

private:
    static void webServerTaskFunction(void *parameter);
    
    TaskHandle_t taskHandle;
    static AsyncWebServer server;
    static DNSServer dnsServer; 
    StackMonitorTask* stackMonitorTask; // Instance of StackMonitorTask
};

#endif