#include "button.h"

Button::Button(int pin)
{
    pinMode(pin, INPUT);
    this->_pin = pin;
    _lastDebounceTime = 0;
    _lastChangeTime = 0;
    _lastStateBtn = HIGH;
}

bool Button::isTimeToUpdate()
{
    return (millis() - _lastDebounceTime) > _delay / 8;
}

void Button::update()
{
    if (!isTimeToUpdate())
        return;
    _lastDebounceTime = millis();
    int btnState = digitalRead(_pin);
    if (btnState == _lastStateBtn)
    {
        _lastChangeTime = millis();
        return;
    };
    if (millis() - _lastChangeTime < _delay)
        return;

    // now at leat least 8 sample points in _delay time changed.
    // so that counts.

    _lastStateBtn = btnState;
}

bool Button::isPressed()
{
    return (_lastStateBtn == HIGH);
}