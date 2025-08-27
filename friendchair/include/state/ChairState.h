#ifndef CHAIR_STATE_H
#define CHAIR_STATE_H

class ChairState {
private:
    int currentState;
    int maxStates;
    static const char* stateNames[];

public:
    ChairState();
    void nextState();
    int getCurrentState() const;
    const char* getCurrentStateName() const;
    void printCurrentState() const;
};

#endif
