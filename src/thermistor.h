#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>

class Thermistor
{
    int pin;
    float beta;
    float nominalR;
    float nominalTC;
    float seriesResistor;
    float calculateTemperatureC(float resistance);

public:
    Thermistor(int _pin, float _seriesResistor, float _beta = 3950.0, float _nominalR = 10000.0, float _nominalTC = 25.0);
    float getTemperatureC();
};

#endif