#ifndef IR_H
#define IR_H

class IR
{
public:
    IR(int leftPin, int rightPin);
    void begin();
    int getLeftSignal();
    int getRightSignal();

private:
    const int _left_pin;
    const int _right_pin;
};

#endif