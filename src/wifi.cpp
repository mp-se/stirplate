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

#include "serial_debug.h"
#include "config.h"
#include "mysecrets.h"      // To be removed when done
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

WiFiManager wifiManager; 

//
//
//
void saveConfigCallback() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Callback to save params." CR));
#endif
    config.saveNeeded = true;
}

//
// Connect to last known access point or create one if connection is not working. 
//
bool Wifi::connect() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connecting to WIFI via connection manager." CR));
    wifiManager.setDebugOutput(true);    
#endif

    // Setup OTA variables
    WiFiManagerParameter cfgOtaUrl("otaUrl", "OTA Base URL (ex: http://server:port/path)", config.otaUrl, sizeof(config.otaUrl));
    wifiManager.addParameter(&cfgOtaUrl);

    // Setup blynk variables
    WiFiManagerParameter cfgBlynkServer("blynkServer", "Blynk server (ex: 192.168.1.1)", config.blynkServer, sizeof(config.blynkServer));
    wifiManager.addParameter(&cfgBlynkServer);
    WiFiManagerParameter cfgBlynkServerPort("blynkServerPort", "Blynk server port (ex: 80)", config.blynkServerPort, sizeof(config.blynkServerPort));
    wifiManager.addParameter(&cfgBlynkServerPort);
    WiFiManagerParameter cfgBlynkToken("blynkToken", "Blynk token (from blynk)", config.blynkToken, sizeof(config.blynkToken));
    wifiManager.addParameter(&cfgBlynkToken);

    wifiManager.setConfigPortalTimeout( WIFI_PORTAL_TIMEOUT );
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // Connect to WIFI
    bool ret = wifiManager.autoConnect( WIFI_DEFAULT_SSID, WIFI_DEFAULT_PWD );

    // If the flag is changed, the callback was triggered
    if( ret && config.saveNeeded ) {
#if LOG_LEVEL==6
        Log.verbose(F("WIFI: Saving configuration options." CR));
#endif
        strcpy( config.otaUrl, cfgOtaUrl.getValue() );
        strcpy( config.blynkServer, cfgBlynkServer.getValue() );
        strcpy( config.blynkServerPort, cfgBlynkServerPort.getValue() );
        strcpy( config.blynkToken, cfgBlynkToken.getValue() );
    }

    config.saveFile();

#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connect returned %s." CR), ret?"True":"False" );
#endif

    return ret;
}

//
// This will erase the stored credentials and forcing the WIFI manager to AP mode.
//
bool Wifi::disconnect() {
    Log.notice(F("WIFI: Erasing stored WIFI credentials." CR));

    // Erase WIFI credentials
    return WiFi.disconnect();
}

#endif

// EOF 