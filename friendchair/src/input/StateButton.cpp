#include "input/StateButton.h"
#include <Arduino.h>

StateButton::StateButton(int pin, ChairState* state) 
    : buttonPin(pin), lastButtonState(HIGH), chairState(state) {
}

void StateButton::init() {
    pinMode(buttonPin, INPUT_PULLUP);
}

void StateButton::update() {
    int buttonState = digitalRead(buttonPin);
    //Serial.print("> Button: ");
    Serial.println(buttonState);

    if (buttonState == LOW && lastButtonState == HIGH) {
        chairState->nextState();
        chairState->printCurrentState();
    }
    lastButtonState = buttonState;
}
