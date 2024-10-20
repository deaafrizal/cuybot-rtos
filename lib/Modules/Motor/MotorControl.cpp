#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide, EEPROMConfig &eepromConfig)
    : _rightSide(rightSide),
        _leftSide(leftSide),
        _eepromConfig(eepromConfig),
        _currentSpeed(0),
        _maxSpeed(255),
        _backwardLimit(200)
        {
            _rightSide.stop();
            _leftSide.stop();
            _turnFactor = _eepromConfig.getMemFloat(10);
            if (_turnFactor <= 0.0f || _turnFactor > 1.0f) {
                _turnFactor = 0.2f;
            }
        }

void MotorControl::forward()
{
    _rightSide.forward(_currentSpeed);
    _leftSide.forward(_currentSpeed);
}

void MotorControl::backward()
{
   int constrainedSpeed = constrain(_currentSpeed, 0, _backwardLimit);
    _rightSide.backward(constrainedSpeed);
    _leftSide.backward(constrainedSpeed);
}

void MotorControl::turnLeft(int speed)
{
   int constrainedSpeed = constrain(_currentSpeed, 0, _backwardLimit);

    if (speed > 0) {
        _rightSide.forward(_currentSpeed);
        _leftSide.forward(_currentSpeed * _turnFactor);
    } else if (speed < 0) {
        _rightSide.backward(constrainedSpeed);
        _leftSide.backward(constrainedSpeed * _turnFactor);
    }
}

void MotorControl::turnRight(int speed)
{
    int constrainedSpeed = constrain(_currentSpeed, 0, _backwardLimit);

    if (speed > 0) {
        _rightSide.forward(_currentSpeed * _turnFactor);
        _leftSide.forward(_currentSpeed);
    } else if (speed < 0) {
        _rightSide.backward(constrainedSpeed * _turnFactor);
        _leftSide.backward(constrainedSpeed);
    }
}

void MotorControl::stop()
{
    _rightSide.stop();
    _leftSide.stop();
}

void MotorControl::setSpeed(int leftSpeed, int rightSpeed) {
    int constrainedLeft = constrain(leftSpeed, 0, _maxSpeed);
    int constrainedRight = constrain(rightSpeed, 0, _maxSpeed);

    _leftSide.forward(constrainedLeft);
    _rightSide.forward(constrainedRight);
}

void MotorControl::setSpeedAndDirection(int speed, int direction)
{
    _currentSpeed = map(abs(speed), 0, 100, 0, _maxSpeed);

    if (speed > 0) {  // Forward
        switch (direction) {
        case 0:
            forward();
            break;
        case -1:
            turnLeft(speed);
            break;
        case 1:
            turnRight(speed);
            break;
        }
    } else if (speed < 0) {  // Backward
        switch (direction) {
        case 0:
            backward();
            break;
        case -1:
            turnLeft(speed);
            break;
        case 1:
            turnRight(speed);
            break;
        }
    } else {
        stop();
    }
}