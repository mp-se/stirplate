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
#include "helper.h"

bool powerLedConfigured = false;

//
//
//
void powerLedConfigure() {
    if( !powerLedConfigured ) {
        pinMode( LED_BUILTIN, OUTPUT );
        powerLedConfigured = true;
    }
}

//
//
//
void powerLedOn() {
    powerLedConfigure();    
    digitalWrite( LED_BUILTIN, 0);
}

//
//
//
void powerLedOff() {
    powerLedConfigure();    
    digitalWrite( LED_BUILTIN, 1);
}

//
//
//
void powerLedToggle() {
    powerLedConfigure();    
    digitalWrite( LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

//
//
//
void printBuildOptions() {
    Log.notice( F("Build options: LOGLEVEL %d " 
#ifdef ACTIVATE_WIFI
                "WIFI "
#endif    
#ifdef ACTIVATE_BLYNK
                "BLYNK "
#endif    
#ifdef ACTIVATE_OTA
                "OTA "
#endif    
#ifdef ACTIVATE_TEMP
                "TEMP "
#endif    
#ifdef SIMULATE_SENSOR
                "SIMULATE_SENSOR "
#endif    
#ifdef SIMULATE_RPM
                "SIMULATE_RPM "
#endif    
#ifdef DISPLAY_SELFTEST    
                "DISPLAY_SELFTEST "
#endif    
    CR), LOG_LEVEL );
}

// EOF 