#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    Buzzer(int buzzerPin);

    void begin();
    void on();
    void off();
    void beep(int duration);
    void playBatteryLowWarning();
    void playCalibrationBeep(int duration);

private:
    int _pin;
    bool _state;
};

#endif