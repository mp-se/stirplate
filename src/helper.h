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
#ifndef _HELPER_H
#define _HELPER_H

// Includes
#include <ArduinoLog.h>

#define LED_FLASH_WIFI 0.2

// Interact with LED
void powerLedConfigure();
void powerLedToggle();
void powerLedOn();
void powerLedOff();
void activateLedTicker(float t);
void deactivateLedTicker();

// Show build options
void printBuildOptions();

// Logging via serial
void printTimestamp(Print* _logOutput);
void printNewline(Print* _logOutput);

// Classes
class SerialDebug {
    public:
        SerialDebug(const long serialSpeed = 115200L);
        static Logging* getLog() { return &Log; };
};

// Global instance created
extern SerialDebug mySerial;

#endif // _HELPER_H

// EOF