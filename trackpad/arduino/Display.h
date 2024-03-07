#ifndef DISPLAY_H
#define DISPLAY_H

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

class Display {
    private:
        ArduinoLEDMatrix matrix;

    public:
        Display();
        void printText(char text[]);
        void printImage(byte frame[][]);
        void clear();
};

#endif