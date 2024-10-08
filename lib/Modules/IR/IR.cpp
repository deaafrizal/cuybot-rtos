#include <IR/IR.h>
#include <Arduino.h>

IR::IR() : _IR_left(0), _IR_right(0) {}

void IR::begin() {
    pinMode(_IR_Left_Pin, INPUT);
    pinMode(_IR_Right_Pin, INPUT);

    _IR_left = digitalRead(_IR_Left_Pin);
    _IR_right = digitalRead(_IR_Right_Pin);
}

void IR::avoidance() {
    //
}

void IR::lineFollowing() {
    //
}

void IR::followMe() {
    //
}

void IR::printLog() {
    Serial.print("IR L: ");
    Serial.println(_IR_left);
    Serial.print("IR R: ");
    Serial.println(_IR_right);
}