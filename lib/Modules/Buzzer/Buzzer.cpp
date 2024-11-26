#include <Buzzer/Buzzer.h>

Buzzer::Buzzer(uint8_t buzzerPin) : _pin(buzzerPin) {}

void Buzzer::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

Buzzer& Buzzer::beep(uint16_t milliseconds) {
    digitalWrite(_pin, HIGH);
    wait(milliseconds);
    digitalWrite(_pin, LOW);
    return *this;
}

void Buzzer::wait(uint16_t milliseconds) {
  delay(milliseconds);
}

void Buzzer::playBatteryLowWarning() {
    for (int i = 0; i < 3; i++) {
        beep(100).wait(200);
    }
}

void Buzzer::playCalibrationBeep(int duration) {
    unsigned long startMillis = millis();
    while (millis() - startMillis < duration) {
        beep(250).wait(250);
    }
}
