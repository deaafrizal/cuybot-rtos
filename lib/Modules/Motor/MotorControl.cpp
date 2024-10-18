#include <Motor/MotorControl.h>

extern float motorTurnFactor;

MotorControl::MotorControl(MotorDriver &rightSide, MotorDriver &leftSide)
    : _rightSide(rightSide),
      _leftSide(leftSide),
      _currentSpeed(0),
      _maxSpeed(255),
      _turnFactor(motorTurnFactor) {
        _rightSide.stop();
        _leftSide.stop();
      }

void MotorControl::forward()
{
    _rightSide.forward(_currentSpeed);
    _leftSide.forward(_currentSpeed);
}

void MotorControl::backward()
{
    _rightSide.backward(_currentSpeed * 0.85);
    _leftSide.backward(_currentSpeed * 0.85);
}

void MotorControl::turnLeft(int speed)
{
    if (speed > 0) {
        _rightSide.forward(_currentSpeed);
        _leftSide.forward(_currentSpeed * _turnFactor);
    } else if (speed < 0) {
        _rightSide.backward(_currentSpeed * 0.85);
        _leftSide.backward(_currentSpeed * _turnFactor);
    }
}

void MotorControl::turnRight(int speed)
{
    if (speed > 0) {
        _rightSide.forward(_currentSpeed * _turnFactor);
        _leftSide.forward(_currentSpeed);
    } else if (speed < 0) {
        _rightSide.backward(_currentSpeed * _turnFactor);
        _leftSide.backward(_currentSpeed * 0.85);
    }
}

void MotorControl::stop()
{
    _rightSide.stop();
    _leftSide.stop();
}

void MotorControl::setSpeed(int leftSpeed, int rightSpeed) {
    _leftSide.forward(constrain(leftSpeed, 0, _maxSpeed));
    _rightSide.forward(constrain(rightSpeed, 0, _maxSpeed));
}

void MotorControl::setSpeedAndDirection(int speed, int direction)
{
    int pwmValue = map(abs(speed), 0, 100, 0, _maxSpeed);

    if (speed > 0) {
        switch (direction)
        {
        case 0:
            _currentSpeed = pwmValue;
            forward();
            break;
        case -1:
            _currentSpeed = pwmValue;
            turnLeft(speed);
            break;
        case 1:
            _currentSpeed = pwmValue;
            turnRight(speed);
            break;
        }
    } else if (speed < 0) {
        switch (direction)
        {
        case 0:
            _currentSpeed = pwmValue;
            backward();
            break;
        case -1: 
            _currentSpeed = pwmValue;
            turnLeft(speed);
            break;
        case 1:
            _currentSpeed = pwmValue;
            turnRight(speed);
            break;
        }
    } else {
        stop();
    }
}