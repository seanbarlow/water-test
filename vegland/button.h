#ifndef MY_BUTTON_H
#define MY_BUTTON_H

#include <Arduino.h>

class Button
{

private:
    int _pin;
    unsigned long _delay = 25;
    unsigned long _lastDebounceTime;
    unsigned long _lastChangeTime;
    int _lastStateBtn;
    bool isTimeToUpdate();

public:
    Button(int pin);

    void init();
    void update();

    bool isPressed();
};

#endif