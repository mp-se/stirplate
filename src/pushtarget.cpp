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
#include "pushtarget.h"
#include "config.h"
#include "helper.h"

#if defined( ACTIVATE_PUSH )

PushTarget myPushTarget;

//
// Send the pressure value
//
void PushTarget::send(float pressure, float temp ) {
    unsigned long timePassed = abs( millis() - ms );
    unsigned long interval = myConfig.getPushInterval()*1000;

    if( ( timePassed < interval ) ) {
#if LOG_LEVEL==6
//      Log.verbose(F("PUSH: Timer has not expired %l %l." CR), timePassed, interval );
#endif
        return;
    }

#if LOG_LEVEL==6
//  Log.verbose(F("PUSH: Sending data." CR) );
#endif
    ms = millis();

    if( myConfig.isHttpActive() )
       sendHttp( pressure, temp );
}

//
// Send data to http endpoint
//
void PushTarget::sendHttp(float rpm, float temp ) {
    // Send data to the standard HTTP endpoint
    Log.notice(F("PUSH: Sending values to http rpm=%F, temp=%F." CR), rpm, temp );

    DynamicJsonDocument doc(200);

    doc["name"]          = myConfig.getMDNS();
    doc["temp"]          = reduceFloatPrecision( temp );
    doc["temp-unit"]     = String( myConfig.getTempFormat() ); 
    doc["rpm"]           = rpm; 
    doc["rssi"]          = WiFi.RSSI(); 

    WiFiClient client;
    HTTPClient http;
    String serverPath = myConfig.getHttpPushTarget();

    // Your Domain name with URL path or IP address with path
    http.begin( client, serverPath);
    String json;
    serializeJson(doc, json);
#if LOG_LEVEL==6
    Log.verbose(F("PUSH: json %s." CR), json.c_str());
#endif

    // Send HTTP GET request
    int httpResponseCode = http.POST(json);
    
    if (httpResponseCode==200) {
        Log.notice(F("PUSH: HTTP Response code %d" CR), httpResponseCode);
    } else {
        Log.error(F("PUSH: HTTP Response code %d" CR), httpResponseCode);
    }

    http.end();
}

#endif 

// EOF 