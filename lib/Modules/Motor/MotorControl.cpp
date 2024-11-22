#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide)
    : _rightSide(rightSide),
      _leftSide(leftSide),
      _maxSpeed(250),
      _backwardLimit(200),
      _currentRightSpeed(0),
      _currentLeftSpeed(0),
      _targetRightSpeed(0),
      _targetLeftSpeed(0),
      _step(10)
{
    _rightSide.stop();
    _leftSide.stop();
}

void MotorControl::setSpeed(int rightSpeed, int leftSpeed) {
    _targetRightSpeed = constrain(rightSpeed, -100, 100);
    _targetLeftSpeed = constrain(leftSpeed, -100, 100);

    // Immediate stop for zero speed
    if (_targetRightSpeed == 0 && _targetLeftSpeed == 0) {
        _currentRightSpeed = 0;
        _currentLeftSpeed = 0;
        _rightSide.stop();
        _leftSide.stop();
        return;
    }

    // Gradual adjustment
    if (_currentRightSpeed < _targetRightSpeed) {
        _currentRightSpeed = min(_currentRightSpeed + _step, _targetRightSpeed);
    } else if (_currentRightSpeed > _targetRightSpeed) {
        _currentRightSpeed = max(_currentRightSpeed - _step, _targetRightSpeed);
    }

    if (_currentLeftSpeed < _targetLeftSpeed) {
        _currentLeftSpeed = min(_currentLeftSpeed + _step, _targetLeftSpeed);
    } else if (_currentLeftSpeed > _targetLeftSpeed) {
        _currentLeftSpeed = max(_currentLeftSpeed - _step, _targetLeftSpeed);
    }

    int pwmRight = map(abs(_currentRightSpeed), 0, 100, 0, _maxSpeed);
    int pwmLeft = map(abs(_currentLeftSpeed), 0, 100, 0, _maxSpeed);

    if (_currentRightSpeed >= 0) {
        _rightSide.forward(pwmRight);
    } else {
        _rightSide.backward(pwmRight);
    }

    if (_currentLeftSpeed >= 0) {
        _leftSide.forward(pwmLeft);
    } else {
        _leftSide.backward(pwmLeft);
    }
}