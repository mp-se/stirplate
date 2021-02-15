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
#ifndef _PWMFAN_H
#define _PWMFAN_H

// Includes
#include <Arduino.h>

// classes
class PwmFan {
    private:
        // A pwm fan can operate on a base frequency of beteween 21 - 28 kHz, the code used 25 kHz as standard but can 
        // easily be changed in the constant below (pwmFrequency)
        //
        // The constants pwmMin/pwmMax is used to control the pwmSignal, on my fan the value of 1024 gives a maximum RPM 
        // of 2700 but in 3liters of liquid its 1400 RPM. 
        //
        // If you want better control of lover RPM you can reduce the pwmMax to a lower value. This would probably require 
        // some testing since it would depend on the FAN and strenght of magnets.
        //
        const static int pwmCtrlPIN = D5;           // Control PWM signal to FAN. D5 (14) PIN on ESP-12F 
        const static int pwmMin = 0;                // Minium value for output
        const static int pwmMax = 1024;             // Maximum value for output
        const static int pwmFrequency = 25000;      // Frequency of 25kHz (Range should be 21-28 kHz)

        volatile int  pwmRotationCounter = 0;       // for the tachiometer
        int           powerPercentage = 0;          // Current output power output in percentage (0-100)
        unsigned long rpmLastMillis;                // used to calculate the RPM 
        int           rpm = 0;

#ifdef SIMULATE_RPM
        unsigned long simualteMillsStart;           // used to return simulation value 
        int simulateRPM();
#endif

    public:
        PwmFan();
        void loop();
        int setPower( int value, int minRange, int maxRange );
        int getCurrentPower() { return powerPercentage; }
        int getCurrentRPM() { return rpm; };
        void tachCallback() { pwmRotationCounter++; }   
};

// Global instance created
extern PwmFan myFan;

#endif // _PWMFAN_H

// EOF