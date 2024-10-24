#include <Buzzer/Buzzer.h>

Buzzer::Buzzer(int buzzerPin) : _pin(buzzerPin), _state(false) {}

void Buzzer::begin() {
    pinMode(_pin, OUTPUT);
    off();
}

void Buzzer::on() {
    digitalWrite(_pin, HIGH);
    _state = true;
}

void Buzzer::off() {
    digitalWrite(_pin, LOW);
    _state = false;
}

void Buzzer::beep(int duration) {
    on();
    delay(duration);
    off();
}

void Buzzer::playBatteryLowWarning() {
    for (int i = 0; i < 3; i++) {
        beep(100);
        delay(200);
    }
}

void Buzzer::playCalibrationBeep(int duration) {
    unsigned long startMillis = millis();
    while (millis() - startMillis < duration) {
        on();
        delay(250);
        off();
        delay(250);
    }
}