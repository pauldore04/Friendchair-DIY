#include "sensor/PressureSensor.h"
#include <Arduino.h>

PressureSensor::PressureSensor(int pin, int thresholdValue) 
    : sensorPin(pin), threshold(thresholdValue), lastState(LOW) {
}

void PressureSensor::init() {
    // Sensor initialisierung - LED wird jetzt vom OutputManager verwaltet
}

void PressureSensor::update() {
    int reading = analogRead(sensorPin);
    //Serial.print("> Sensor: ");
    Serial.println(reading);

    int currentState = (reading >= threshold) ? HIGH : LOW;
    
    // LED-Steuerung wurde zum OutputManager verschoben

    if (currentState != lastState) {
        lastState = currentState;
        
        if (currentState == HIGH) {
            Serial.println("Someone is sitting on the chair.");
        } else {
            Serial.println("No one is sitting on the chair.");
        }
    }
}

bool PressureSensor::isPressed() const {
    return lastState == HIGH;
}
