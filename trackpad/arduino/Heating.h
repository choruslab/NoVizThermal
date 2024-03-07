#ifndef HEATING_H
#define HEATING_H

#include "Temperature.h"
#include "Peltier.h"

class Heating {
    private:
        Temperature temperature;
        Peltier peltier;
        int maxTemp;

    public:
        Heating(int maxTemp);
        void regulateTemperature();



};

#endif