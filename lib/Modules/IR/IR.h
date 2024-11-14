#ifndef IR_H
#define IR_H

class IR
{
public:
    IR(int leftPin, int middlePin, int rightPin);
    void begin();
    int getLeftSignal();
    int getRightSignal();
    int getMiddleSignal();

private:
    const int _left_pin;
    const int _middle_pin;
    const int _right_pin;
};

#endif