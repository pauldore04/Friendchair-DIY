#include "output/OutputManager.h"
#include <Arduino.h>

OutputManager::OutputManager() : internalLedPin(2), externalLedPin(-1), isInitialized(false) {
}

void OutputManager::init(int externalPin, int internalPin) {
    externalLedPin = externalPin;
    internalLedPin = internalPin;
    
    // Externe LED initialisieren
    pinMode(externalLedPin, OUTPUT);
    digitalWrite(externalLedPin, LOW);
    
    // Interne LED initialisieren
    pinMode(internalLedPin, OUTPUT);
    digitalWrite(internalLedPin, LOW);
    
    isInitialized = true;
    Serial.printf("OutputManager initialized - External: %d, Internal: %d\n", 
                  externalLedPin, internalLedPin);
}

void OutputManager::setExternalLed(bool state) {
    if (!isInitialized) return;
    digitalWrite(externalLedPin, state ? HIGH : LOW);
}

void OutputManager::toggleExternalLed() {
    if (!isInitialized) return;
    static bool currentState = false;
    currentState = !currentState;
    digitalWrite(externalLedPin, currentState ? HIGH : LOW);
}

void OutputManager::setInternalLed(bool state) {
    digitalWrite(internalLedPin, state ? HIGH : LOW);
}

void OutputManager::blinkInternalLed() {
    static bool ledState = false;
    ledState = !ledState;
    digitalWrite(internalLedPin, ledState ? HIGH : LOW);
}

void OutputManager::toggleInternalLed() {
    static bool currentState = false;
    currentState = !currentState;
    digitalWrite(internalLedPin, currentState ? HIGH : LOW);
}

bool OutputManager::getExternalLedState() const {
    if (!isInitialized) return false;
    return digitalRead(externalLedPin) == HIGH;
}

bool OutputManager::getInternalLedState() const {
    return digitalRead(internalLedPin) == HIGH;
}
