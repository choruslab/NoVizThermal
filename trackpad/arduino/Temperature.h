#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Wire.h>
#include <SparkFunTMP102.h>

class Temperature {
    private:
        TMP102 sensor;
        int alertPin;

    public:
        Temperature(int alertPin);

}

#endif