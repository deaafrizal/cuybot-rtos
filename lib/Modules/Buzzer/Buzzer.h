#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

#define BUZZER_PIN 1

class Buzzer {
public:
    Buzzer();
    void begin();
    void on();
    void off();
    void beep(int duration);
    bool getState();
    void setState(bool state);

private:
    const int _buzzer_pin = BUZZER_PIN;
    bool _state;
};

#endif