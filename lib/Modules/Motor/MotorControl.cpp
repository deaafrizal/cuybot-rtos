#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide)
    : _rightSide(rightSide),
        _leftSide(leftSide),
        _maxSpeed(250),
        _backwardLimit(200)
        {
            _rightSide.stop();
            _leftSide.stop();
        }

void MotorControl::setSpeed(int rightSpeed, int leftSpeed) {
    rightSpeed = constrain(rightSpeed, -90, 90);
    leftSpeed = constrain(leftSpeed, -90, 90);
    
    int pwmRight = map(abs(rightSpeed), 0, 90, 0, _maxSpeed);
    int pwmLeft = map(abs(leftSpeed), 0, 90, 0, _maxSpeed);

    if (rightSpeed > 0) {
        _rightSide.forward(pwmRight);
    } else if (rightSpeed < 0) {
        _rightSide.backward(pwmRight);
    } else {
        _rightSide.stop();
    }

    if (leftSpeed > 0) {
        _leftSide.forward(pwmLeft);
    } else if (leftSpeed < 0) {
        _leftSide.backward(pwmLeft);
    } else {
        _leftSide.stop();
    }
}