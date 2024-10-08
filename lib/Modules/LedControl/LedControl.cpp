#include <LedControl/LedControl.h>
#include <Arduino.h>

// Constructor to set pin modes
LedControl::LedControl(int pin_A, int pin_B) : _pin_A(pin_A), _pin_B(pin_B) {
    pinMode(_pin_A, OUTPUT);
    pinMode(_pin_B, OUTPUT);
}

// Destructor
LedControl::~LedControl() {
    // Nothing specific for now
}

// Turn on a specific LED
void LedControl::turnOn(int led) {
    digitalWrite(led, HIGH);
}

// Turn off a specific LED
void LedControl::turnOff(int led) {
    digitalWrite(led, LOW);
}

// Apply state to the LEDs dynamically
void LedControl::applyLedState(bool state_A, bool state_B) {
    digitalWrite(_pin_A, state_A ? HIGH : LOW);
    digitalWrite(_pin_B, state_B ? HIGH : LOW);
}

// Dynamic setMode based on the input mode
void LedControl::setMode(int mode) {
    switch (mode) {
        case 1:  // RED -> Mode 1
            applyLedState(LOW, LOW);
            break;
        case 2:  // GREEN -> Mode 2
            applyLedState(HIGH, LOW);
            break;
        case 3:  // LIGHTBLUE -> Mode 3
            applyLedState(LOW, HIGH);
            break;
        case 4:  // YELLOW -> Mode 4
            applyLedState(HIGH, HIGH);
            break;
        default:
            // Default case if mode is not recognized
            Serial.println("Invalid mode");
            applyLedState(LOW, LOW);  // Default to Mode 1 (OFF)
            break;
    }
}