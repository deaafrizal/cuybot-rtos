#include <LedControl/LedControl.h>
#include <Arduino.h>

LedControl::LedControl(int pin_A, int pin_B) : _pin_A(pin_A), _pin_B(pin_B) {
    pinMode(_pin_A, OUTPUT);
    pinMode(_pin_B, OUTPUT);
}

LedControl::~LedControl() {}

void LedControl::turnOn(int led) {
    digitalWrite(led, HIGH);
}

void LedControl::turnOff(int led) {
    digitalWrite(led, LOW);
}

void LedControl::applyLedState(bool state_A, bool state_B) {
    digitalWrite(_pin_A, state_A ? HIGH : LOW);
    digitalWrite(_pin_B, state_B ? HIGH : LOW);
}

void LedControl::setMode(int mode) {
    switch (mode) {
        case 1:  // Mode 1
            applyLedState(LOW, LOW);
            break;
        case 2:  // Mode 2
            applyLedState(HIGH, LOW);
            break;
        case 3:  // Mode 3
            applyLedState(LOW, HIGH);
            break;
        case 4:  // Mode 4
            applyLedState(HIGH, HIGH);
            break;
        default:
            Serial.println("Invalid mode");
            applyLedState(LOW, LOW);  // Default to Mode 1
            break;
    }
}