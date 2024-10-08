#include <Motor/MotorControl.h>

MotorControl::MotorControl(MotorDriver &rightMotor, MotorDriver &leftMotor)
    : _rightMotor(rightMotor),
      _leftMotor(leftMotor),
      _currentSpeed(0),
      _maxSpeed(255),
      _turnFactor(0.15) {
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

void MotorControl::turnLeft()
{
    _rightMotor.motorRev(_currentSpeed);
    _leftMotor.motorGo(_currentSpeed);
}

void MotorControl::turnRight()
{
    _rightMotor.motorGo(_currentSpeed);
    _leftMotor.motorRev(_currentSpeed);
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

void MotorControl::setDirectionAndSpeed(int speed, char direction)
{
    setSpeed(speed);

    switch (direction)
    {
    case 0:
        stop();
        break;
    case 1:
        forward();
        break;
    case 2:
        backward();
        break;
    case 3:
        turnRight();
        break;
    case 4:
        turnLeft();
        break;
    default:
        stop();
        break;
    }
}