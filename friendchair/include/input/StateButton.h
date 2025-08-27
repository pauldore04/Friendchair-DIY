#ifndef STATE_BUTTON_H
#define STATE_BUTTON_H

#include "state/ChairState.h"

class StateButton {
private:
    int buttonPin;
    int lastButtonState;
    ChairState* chairState;

public:
    StateButton(int pin, ChairState* state);
    void init();
    void update();
};

#endif
