#include "thermistor.h"

Thermistor::Thermistor(int _pin, float _seriesResistor, float _beta, float _nominalR, float _nominalTC)
{
    pin = _pin;
    beta = _beta;
    nominalR = _nominalR;
    nominalTC = _nominalTC;
    seriesResistor = _seriesResistor;
    pinMode(pin, INPUT);
}

float Thermistor::getTemperatureC()
{
    float reading = analogRead(pin);
    float resistance = seriesResistor / (1023.0 / reading - 1);
    return calculateTemperatureC(resistance);
}

float Thermistor::calculateTemperatureC(float resistance)
{
    float steinhart;
    steinhart = resistance / nominalR;      // (R/Ro)
    steinhart = log(steinhart);             // ln(R/Ro)
    steinhart /= beta;                      // 1/B * ln(R/Ro)
    steinhart += 1.0 / (nominalTC + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;            // Invert
    steinhart -= 273.15;                    // Convert to Celsius
    return steinhart;
}