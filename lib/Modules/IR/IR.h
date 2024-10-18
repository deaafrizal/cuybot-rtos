#ifndef IR_H
#define IR_H

#define IR_LEFT_PIN 10
#define IR_RIGHT_PIN 7

class IR
{
    public:
        IR();
        void begin();
        void printLog();
        int getIRLeft();
        int getIRRight();
        
    private:
        const int _IR_Left_Pin = IR_LEFT_PIN;
        const int _IR_Right_Pin = IR_RIGHT_PIN;
        int _IR_left;
        int _IR_right;
};

#endif