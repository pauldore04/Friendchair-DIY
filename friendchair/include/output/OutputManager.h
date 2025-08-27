#ifndef OUTPUT_MANAGER_H
#define OUTPUT_MANAGER_H

class OutputManager {
private:
    int internalLedPin;  // Konfigurierbare Built-in LED
    int externalLedPin;
    bool isInitialized;

public:
    OutputManager();
    void init(int externalPin, int internalPin = 2);
    
    // Externe LED Steuerung (für Sensor/Network)
    void setExternalLed(bool state);
    void toggleExternalLed();
    
    // Interne LED Steuerung (für Network Status)
    void setInternalLed(bool state);
    void blinkInternalLed();
    void toggleInternalLed();
    
    // Utility
    bool getExternalLedState() const;
    bool getInternalLedState() const;
};

#endif
