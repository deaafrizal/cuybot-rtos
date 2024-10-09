#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightMotor, MotorDriver &leftMotor)
    : _rightMotor(rightMotor),
      _leftMotor(leftMotor),
      _currentSpeed(0),
      _maxSpeed(255),
      _turnFactor(0.5) {
        _rightMotor.motorBrake();
        _leftMotor.motorBrake();
      }

void MotorControl::forward()
{
    _rightMotor.motorGo(_currentSpeed);
    _leftMotor.motorGo(_currentSpeed);
}

void MotorControl::backward()
{
    _rightMotor.motorRev(_currentSpeed);
    _leftMotor.motorRev(_currentSpeed);
}

void MotorControl::turnLeft(int speed)
{
    Serial.print("right curspeed: ");
    Serial.println(_currentSpeed);

    if (speed > 0) {
        Serial.println("Forward turn left");

        _rightMotor.motorGo(_currentSpeed);
        _leftMotor.motorGo(_currentSpeed * _turnFactor);
    } else if (speed < 0) {
        Serial.println("Backward turn left");

        _rightMotor.motorRev(_currentSpeed);
        _leftMotor.motorRev(_currentSpeed * _turnFactor);
    }
}

void MotorControl::turnRight(int speed)
{
    Serial.print("right curspeed: ");
    Serial.println(_currentSpeed);

    if (speed > 0) {
        Serial.println("Forward turn right");

        _rightMotor.motorGo(_currentSpeed * _turnFactor);
        _leftMotor.motorGo(_currentSpeed);
    } else if (speed < 0) {
        Serial.println("Backward turn right");

        _rightMotor.motorRev(_currentSpeed * _turnFactor);
        _leftMotor.motorRev(_currentSpeed);
    }
}

void MotorControl::brake()
{
    _rightMotor.motorBrake();
    _leftMotor.motorBrake();
}

void MotorControl::stop()
{
    _rightMotor.motorStop();
    _leftMotor.motorStop();
}

void MotorControl::setSpeed(int speed)
{
    _currentSpeed = constrain(speed, 0, _maxSpeed);
}

void MotorControl::setDirectionAndSpeed(int speed, int direction)
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