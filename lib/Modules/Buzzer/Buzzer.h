#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class Buzzer {
public:
    Buzzer(int pin);         // Constructor to initialize the buzzer with the pin
    void begin();            // Initialize the buzzer pin
    void on();               // Turn the buzzer on
    void off();              // Turn the buzzer off
    void beep(int duration); // Beep for a specific duration (non-blocking)

private:
    int _pin;                // Pin to which the buzzer is connected
    bool _state;             // Current state of the buzzer (on or off)
};

#endif