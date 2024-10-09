#include <Buzzer/Buzzer.h>

Buzzer::Buzzer(int pin) : _pin(pin), _state(false) {}

void Buzzer::begin() {
    pinMode(_pin, OUTPUT);
    off();  // Ensure the buzzer is off initially
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
    on();  // Turn the buzzer on
    vTaskDelay(pdMS_TO_TICKS(duration));  // Use non-blocking delay
    off();  // Turn the buzzer off after the delay
}