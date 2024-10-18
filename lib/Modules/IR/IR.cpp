#include <IR/IR.h>
#include <Arduino.h>

IR::IR() {}

void IR::begin() {
    pinMode(_IR_Left_Pin, INPUT);
    pinMode(_IR_Right_Pin, INPUT);
    
    _IR_left = digitalRead(_IR_Left_Pin);
    _IR_right = digitalRead(_IR_Right_Pin);
}

int IR::getIRLeft() {
    _IR_left = digitalRead(_IR_Left_Pin);
    return _IR_left;
}

int IR::getIRRight() {
    _IR_right = digitalRead(_IR_Right_Pin);
    return _IR_right;
}

void IR::printLog() {
    _IR_left = digitalRead(_IR_Left_Pin);
    _IR_right = digitalRead(_IR_Right_Pin);

    Serial.print("IR L: ");
    Serial.println(_IR_left);
    Serial.print("IR R: ");
    Serial.println(_IR_right);
}