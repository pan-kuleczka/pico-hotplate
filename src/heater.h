#ifndef HEATER_H
#define HEATER_H

#include <PID_v1.h>
#include <Arduino.h>

class Heater
{
    bool enabled = false;
    int pin;
    PID pid;
    double input, output, setpoint;
public:
    Heater(int _pin, float kp = 2, float ki = 5, float kd = 1);
    void setTemperature(float temperature);
    void updateTemperature(float temperature);
    void update();
    void enable();
    void disable();
    bool isEnabled();
    float getPowerUsage(float maxPower);
};

#endif
