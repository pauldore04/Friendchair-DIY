#include "state/ChairState.h"
#include <Arduino.h>

const char* ChairState::stateNames[] = {
    "idle",
    "movie",
    "gaming",
    "music",
};

ChairState::ChairState() : currentState(0), maxStates(4) {
}

void ChairState::nextState() {
    currentState = (currentState + 1) % maxStates;
}

int ChairState::getCurrentState() const {
    return currentState;
}

const char* ChairState::getCurrentStateName() const {
    return stateNames[currentState];
}

void ChairState::printCurrentState() const {
    Serial.print("Friend's Chair State: ");
    Serial.println(stateNames[currentState]);
}
