#include <Motor/MotorControl.h>

EEPROMConfig MotorControl::_eepromConfig;

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide)
    : _rightSide(rightSide),
        _leftSide(leftSide),
        _currentSpeed(0),
        _maxSpeed(255),
        _backwardLimit(180)
        {
            _rightSide.stop();
            _leftSide.stop();
            _turnFactor = _eepromConfig.getMemFloat(10);
        }

float MotorControl::calculateTurnFactor() {
    float minTurnFactor = 0.0f;
    float maxTurnFactor = 0.4f;

    float speedFraction = map(abs(_currentSpeed), 0, 255, 0, 100) / 100.0f;

    return minTurnFactor + (maxTurnFactor - minTurnFactor) * speedFraction;
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
    _turnFactor = calculateTurnFactor();
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
    _turnFactor = calculateTurnFactor();
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
    // speed from websocket: -90, 0, 90 = 0 - 255
    // currentspeed converting websocket data to pwm: 0 - 255
    _currentSpeed = map(abs(speed), 0, 90, 0, _maxSpeed);
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