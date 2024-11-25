#include "heater.h"

Heater::Heater(int _pin, float kp, float ki, float kd) : pid(&input, &output, &setpoint, kp, ki, kd, DIRECT)
{
    pin = _pin;
    pid.SetMode(AUTOMATIC);
    pinMode(pin, OUTPUT);
}

void Heater::setTemperature(float temperatureSetpoint)
{
    setpoint = temperatureSetpoint;
}

void Heater::updateTemperature(float measuredTemperature)
{
    input = measuredTemperature;
}

void Heater::update()
{
    pid.Compute();
    if(enabled)
    {
        analogWrite(pin, output);
    }
    else
    {
        analogWrite(pin, 0);
    }
}

void Heater::enable()
{
    enabled = true;
}

void Heater::disable()
{
    enabled = false;
}

bool Heater::isEnabled()
{
    return enabled;
}

float Heater::getPowerUsage(float maxPower)
{
    return output / 255 * maxPower;
}
