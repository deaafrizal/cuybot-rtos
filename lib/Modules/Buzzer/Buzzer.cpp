#include <Buzzer/Buzzer.h>

Buzzer::Buzzer() : _state(false) {}

void Buzzer::begin() {
    pinMode(_buzzer_pin, OUTPUT);
    off();
}

void Buzzer::on() {
    digitalWrite(_buzzer_pin, HIGH);
}

void Buzzer::off() {
    digitalWrite(_buzzer_pin, LOW);
}

void Buzzer::beep(int duration) {
    on();
    delay(duration);
    off();
}


bool Buzzer::getState() {
    return _state;
}

void Buzzer::setState(bool state) {
    _state = state;
}