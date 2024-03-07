#ifndef PELTIER_H
#define PELTIER_H

class Peltier {
    private:
        int pin;
        bool active;
    
    public:
        Peltier(int pin);
        bool isPeltierActive();
        void turnOn();
        void turnOff();
};


#endif