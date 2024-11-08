#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide)
    : _rightSide(rightSide),
        _leftSide(leftSide),
        _maxSpeed(255)
        {
            _rightSide.stop();
            _leftSide.stop();
        }

void MotorControl::setSpeed(int rightSpeed, int leftSpeed) {
    int pwmLeft = map(leftSpeed, -100, 100, -_maxSpeed, _maxSpeed);
    int pwmRight = map(rightSpeed, -100, 100, -_maxSpeed, _maxSpeed);

    if (pwmLeft < 0) {
        _leftSide.backward(abs(pwmLeft * 0.5));
    } else if (pwmLeft > 0) {
        _leftSide.forward(pwmLeft);
    } else {
        _leftSide.forward(0);
    }

    if (pwmRight < 0) {
        _rightSide.backward(abs(pwmRight * 0.5));
    } else if (pwmRight > 0) {
        _rightSide.forward(pwmRight);
    } else {
        _rightSide.forward(0);
    }
}