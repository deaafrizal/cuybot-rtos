#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
public:
    Buzzer(uint8_t buzzerPin);

    void begin();
    Buzzer& beep(uint16_t milliseconds);
    void wait(uint16_t milliseconds);
    void playBatteryLowWarning();
    void playCalibrationBeep(int duration);

private:
    uint8_t _pin;
};

#endif
