#ifndef LEDCONTROL_H
#define LEDCONTROL_H

class LedControl
{
public:
    LedControl(int pin_A, int pin_B);
    ~LedControl();

    void begin();
    void turnOn(int led);
    void turnOff(int led);
    void setMode(int mode);

private:
    const int _pin_A;
    const int _pin_B;

    void applyLedState(bool state_A, bool state_B);
};

#endif