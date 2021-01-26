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
#include "blynk.h"
#include "serial_debug.h"

#ifdef ACTIVATE_BLYNK

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

BlynkPins blynk;

//
// These are callback functions for updates from the Mobile App. This is the power callback (0-100%).
//
BLYNK_WRITE(V3)
{
    int v = param.asInt(); // assigning incoming value from pin V1 to a variable
    //Log.verbose(F("BLYN: Callback V3 %d." CR), v);
    blynk.setRemotePower( v );
}

//
// These are callback functions for updates from the Mobile App. This is the remote activate toggle (0/1).
//
BLYNK_WRITE(V2)
{
    int v = param.asInt(); // assigning incoming value from pin V1 to a variable
    //Log.verbose(F("BLYN: Callback V2 %d." CR), v);
    blynk.setRemoteToggle( v );
}

//
// Send the RPM value to the blynk server on pin V0
//
void BlynkPins::writeRemoteRPM(int v) {
    //Log.verbose(F("BLYN: write rpm %d." CR), v);
    Blynk.virtualWrite(V0, v);
}

//
// Send the power value (0-100%) to the blynk server on pin V1
//
void BlynkPins::writeRemotePower(int v) {
    //Log.verbose(F("BLYN: write power %d." CR), v);
    Blynk.virtualWrite(V1, v);
}

//
// Connect to the WIFI and blynk server
//
void BlynkPins::connect(const char* wifiName, const char* wifiPwd, const char* blynkToken, IPAddress ip, uint16_t port) {
    //Log.verbose(F("BLYN: connect." CR));
    Blynk.begin(wifiName, wifiPwd, blynkToken, ip, port);
}

//
// Call the run method in the blynk library (should be called in the loop)
//
void BlynkPins::run() {
    //Log.verbose(F("BLYN: run." CR));

    Blynk.run();
}

#endif 

// EOF 