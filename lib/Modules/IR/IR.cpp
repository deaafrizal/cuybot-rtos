#include <IR/IR.h>
#include <Arduino.h>

IR::IR(int leftPin, int middlePin, int rightPin): _left_pin(leftPin), _middle_pin(middlePin), _right_pin(rightPin) {}

void IR::begin() {
    pinMode(_left_pin, INPUT);
    pinMode(_right_pin, INPUT);
}

int IR::getLeftSignal() {
    return digitalRead(_left_pin);
}

int IR::getRightSignal() {
    return digitalRead(_right_pin);
}

int IR::getMiddleSignal() {
    return digitalRead(_middle_pin);
}