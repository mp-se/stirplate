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

#if defined( ACTIVATE_BLYNK ) && defined( ACTIVATE_WIFI )

#if LOG_LEVEL==6
#define BLYNK_PRINT Serial
#endif

#include "helper.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkPins blynk;

//
// These are callback functions for updates from the Mobile App. This is the power callback (0-100%).
//
BLYNK_WRITE(V3)
{
    int v = param.asInt(); // assigning incoming value from pin V1 to a variable
#if LOG_LEVEL==6
    Log.verbose(F("BLYN: Callback V3 %d." CR), v);
#endif
    blynk.setRemotePower( v );
}

//
// These are callback functions for updates from the Mobile App. This is the remote activate toggle (0/1).
//
BLYNK_WRITE(V2)
{
    int v = param.asInt(); // assigning incoming value from pin V1 to a variable
#if LOG_LEVEL==6
    Log.verbose(F("BLYN: Callback V2 %d." CR), v);
#endif
    blynk.setRemoteToggle( v );
}

//
// Send the RPM value to the blynk server on pin V0
//
void BlynkPins::writeRemoteRPM(int v) {
//#if LOG_LEVEL==6
//    Log.verbose(F("BLYN: write rpm %d." CR), v);
//#endif
    Blynk.virtualWrite(V0, v);
}

//
// Send the power value (0-100%) to the blynk server on pin V1
//
void BlynkPins::writeRemotePower(int v) {
//#if LOG_LEVEL==6
//    Log.verbose(F("BLYN: write power %d." CR), v);
//#endif
    Blynk.virtualWrite(V1, v);
}

//
// Inform blynk about what version we are running
//
void BlynkPins::writeRemoteVer(const char *ver) {
//#if LOG_LEVEL==6
//    Log.verbose(F("BLYN: write version %s." CR), ver);
//#endif
    Blynk.virtualWrite(V4, ver);
}

//
// Connect to the WIFI and blynk server
//
void BlynkPins::connect(const char* blynkToken, const char *ip, int port) {
    IPAddress bs(0,0,0,0);
    bs.fromString(ip);
#if LOG_LEVEL==6
    Log.verbose(F("BLYN: connect %s." CR), bs.toString().c_str());
#endif    
    Blynk.config(blynkToken, bs, (uint16_t) port);
    Blynk.run();
    Blynk.syncAll();
    Blynk.run();
}

//
// Call the run method in the blynk library (should be called in the loop)
//
void BlynkPins::loop() {
    // Dont put serial debug output in this call
    Blynk.run();
}

#endif // ACTIVATE_BLYNK && ACTIVATE_WIFI 

// EOF 