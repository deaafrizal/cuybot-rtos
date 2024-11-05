#ifndef LEDCONROL_H
#define LEDCONROL_H

#define LED_STATE_1 2
#define LED_STATE_2 8

class LedControl
{
public:
    LedControl(int pin_A = LED_STATE_1, int pin_B = LED_STATE_2);
    ~LedControl();

    void turnOn(int led);
    void turnOff(int led);
    void setMode(int mode);  // Dynamic LED Interaction

private:
    const int _pin_A;
    const int _pin_B;

    void applyLedState(bool state_A, bool state_B);
};

#endif