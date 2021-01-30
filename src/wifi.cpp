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
#include "wifi.h"

#ifdef ACTIVATE_WIFI

//
//
//
bool Wifi::connect(const char *ap, const char *pwd) {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connecting to %s." CR), ap);
#endif
    WiFi.setAutoReconnect( true );
    WiFi.begin( ap, pwd);

    // TODO: Add a timeout if we are not able to connect to the WIFI
    // TODO: Do error checking in and return false if needed

    Log.notice(F("WIFI: Connecting." CR));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Log.notice(F("WIFI: ." CR));
    }

    Log.notice(F("WIFI: Connected to %s with %s." CR), ap, WiFi.localIP().toString().c_str());
    return true;
}

//
//
//
bool Wifi::disconnect() {

    // Not yet implemented (not needed)
    return true;
}

#endif

// EOF 