#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

class PressureSensor {
private:
    int sensorPin;
    int threshold;
    int lastState;

public:
    PressureSensor(int pin, int thresholdValue);
    void init();
    void update();
    bool isPressed() const;
};

#endif
