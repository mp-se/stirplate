/*
MIT License

Copyright (c) 2021 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#ifndef _ANALOGSENSOR_H
#define _ANALOGSENSOR_H

// Includes
#include <Arduino.h>

// classes
class AnalogSensor {
    private:
        const static int analogPIN = A0;    // A0 PIN on ESP-12
        int value = 0;                      // last value read from sensor              
        
#ifdef SIMULATE_SENSOR
        unsigned long simualteMillsStart;   // used to return simulation value 
        int simulateSensor();
#endif

    public:
        void setup();
        int  readSensor();
};

// Global instance created
extern AnalogSensor stirAnalogSensor;

#endif // _ANALOGSENSOR_H

// EOF