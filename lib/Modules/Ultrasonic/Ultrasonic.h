#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#define TRIGGER_PIN 20
#define ECHO_PIN 21

class Ultrasonic
{
    public:
        Ultrasonic();
        void begin();
        long getDistance();

    private:
        const int _trigger_pin = TRIGGER_PIN;
        const int _echo_pin = ECHO_PIN;
};

#endif