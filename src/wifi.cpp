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

#if defined( ACTIVATE_WIFI )

#include "config.h"
#include "helper.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

Wifi myWifi;
WiFiManager myWifiManager; 
ESP8266WebServer myWebServer(80);

//
// Callback from wifiManager when settings have changed.
//
void saveConfigCallback() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: wifiMgr callback to save params." CR));
#endif
    myConfig.saveNeeded = true;
}

//
// Callback from webServer when / has been accessed.
//
void webHandleRoot() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /." CR));
#endif
    char buf[100];
    sprintf( &buf[0], "%s (%s), %s", CFG_APPNAME, CFG_APPVER, myConfig.mDNS );
    myWebServer.send(200, "text/plain", buf );
}

//
// Callback from webServer when / has been accessed.
//
void webHandleConfig() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /config." CR));
#endif
    StaticJsonDocument<512> doc;
    myConfig.createJson( doc );
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
#endif    
    String out;
    serializeJson(doc, out);
    myWebServer.send(200, "application/json", out.c_str() );
}

//
// Callback from webServer when / has been accessed.
//
void webHandleReset() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /reset." CR));
#endif
    myWebServer.send(200, "text/plain", "Clearing WIFI credentials, doing reset...");
    delay(1000);
    WiFi.disconnect();  // Clear credentials
    ESP.reset();
    delay(500);
}

//
// Setup the Web Server callbacks and start it
//
void Wifi::setupWebServer() {
//#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Setting up web server." CR));
//#endif
    myWebServer.on("/", webHandleRoot);
    myWebServer.on("/config", webHandleConfig);
    myWebServer.on("/reset", webHandleReset);
    myWebServer.begin();
}

//
// Connect to last known access point or create one if connection is not working. 
//
bool Wifi::connect( bool showPortal ) {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connecting to WIFI via connection manager (portal=%s)." CR), showPortal?"true":"false");
    myWifiManager.setDebugOutput(true);    
#endif

    // LED will show that we are in WIFI connection/configuration mode
    activateLedTicker( LED_FLASH_WIFI );

    // Setup OTA variables
    WiFiManagerParameter cfgOtaUrl("otaUrl", "OTA Base URL (ex: http://server:port/path)", myConfig.otaUrl, sizeof(myConfig.otaUrl)-1);
    myWifiManager.addParameter(&cfgOtaUrl);

    // Setup Blynk variables
    WiFiManagerParameter cfgBlynkServer("blynkServer", "Blynk server (ex: 192.168.1.1)", myConfig.blynkServer, sizeof(myConfig.blynkServer)-1);
    myWifiManager.addParameter(&cfgBlynkServer);
    WiFiManagerParameter cfgBlynkServerPort("blynkServerPort", "Blynk server port (ex: 8080)", myConfig.blynkServerPort, sizeof(myConfig.blynkServerPort)-1);
    myWifiManager.addParameter(&cfgBlynkServerPort);
    WiFiManagerParameter cfgBlynkToken("blynkToken", "Blynk token (from blynk)", myConfig.blynkToken, sizeof(myConfig.blynkToken)-1);
    myWifiManager.addParameter(&cfgBlynkToken);

    // Setup temp format
    WiFiManagerParameter tempFormat("tempFormat", "Temperature format (C|F)", myConfig.tempFormat, sizeof(myConfig.tempFormat)-1);
    myWifiManager.addParameter(&tempFormat);

    myWifiManager.setConfigPortalTimeout( WIFI_PORTAL_TIMEOUT );
    myWifiManager.setSaveConfigCallback(saveConfigCallback);

    // Connect to WIFI
   // Connect to WIFI
    if( showPortal )
        connectedFlag = myWifiManager.startConfigPortal( WIFI_DEFAULT_SSID, WIFI_DEFAULT_PWD );
    else
        connectedFlag = myWifiManager.autoConnect( WIFI_DEFAULT_SSID, WIFI_DEFAULT_PWD );

    // If the flag is changed, the callback was triggered
    if( connectedFlag && myConfig.saveNeeded ) {
#if LOG_LEVEL==6
        Log.verbose(F("WIFI: Saving configuration options." CR));
#endif
        const char* t = cfgOtaUrl.getValue();
        if( strcmp(t, "C")==0 || strcmp(t, "c")==0 )
            strcpy( myConfig.tempFormat, "C" );
        if( strcmp(t, "F")==0 || strcmp(t, "f")==0 )
            strcpy( myConfig.tempFormat, "F" );

        strcpy( myConfig.otaUrl, cfgOtaUrl.getValue() );
        strcpy( myConfig.blynkServer, cfgBlynkServer.getValue() );
        strcpy( myConfig.blynkServerPort, cfgBlynkServerPort.getValue() );
        strcpy( myConfig.blynkToken, cfgBlynkToken.getValue() );
    }

    myConfig.saveFile();

#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connect returned %s." CR), connectedFlag?"True":"False" );
#endif

    if( connectedFlag ) {
#if LOG_LEVEL==6
        Log.verbose(F("WIFI: Starting mDNS for %s." CR), myConfig.mDNS );
#endif
        MDNS.begin( myConfig.mDNS );
        MDNS.addService("http", "tcp", 80);
        setupWebServer();

        // Disable the ticker if we are connected, otherwise it's a signal that wifi connection failed.
        deactivateLedTicker();
    }

    return connectedFlag;
}

//
// called from main loop
//
void Wifi::loop() {
    // Dont put serial debug output in this call
    myWebServer.handleClient();
    MDNS.update();
}

//
// This will erase the stored credentials and forcing the WIFI manager to AP mode.
//
bool Wifi::disconnect() {
    Log.notice(F("WIFI: Erasing stored WIFI credentials." CR));
    // Erase WIFI credentials
    return WiFi.disconnect();
}

#endif // ACTIVATE_WIFI

#if defined( ACTIVATE_OTA ) && defined( ACTIVATE_WIFI )

//
//
//
bool Wifi::updateFirmware() {

    if( !newFirmware ) {
        Log.notice(F("WIFI: No newer version exist, skipping update." CR));
        return false;
    }

//#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Updating firmware." CR));
//#endif

    WiFiClient client;
    String serverPath = myConfig.otaUrl;
    serverPath += "firmware.bin";

    // TODO: Update code to work with https connection 

    HTTPUpdateResult ret = ESPhttpUpdate.update(client, serverPath);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("WIFI: Updating failed %d, %s." CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            break;

        case HTTP_UPDATE_OK:
            Log.notice("WIFI: Updated succesfull, rebooting." );
            ESP.reset();
            break;
    }

    return false;
}

//
// Check what firmware version is available over OTA
//
bool Wifi::checkFirmwareVersion() {

    if( !myConfig.isOtaConfigured() ) {
        Log.notice(F("WIFI: No OTA URL defined, skipping." CR));
        return false;
    }

//#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Checking if new version exist." CR));
//#endif
    WiFiClient client;
    HTTPClient http;
    String serverPath = myConfig.otaUrl;
    serverPath += "version.json";

    // Your Domain name with URL path or IP address with path
    http.begin( client, serverPath);
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
      
    if (httpResponseCode==200) {
        Log.notice(F("WIFI: HTTP Response code %d" CR), httpResponseCode);
 
        String payload = http.getString();
//#if LOG_LEVEL==6
//        Log.verbose(F("WIFI: Payload %s." CR), payload.c_str());
//#endif
        StaticJsonDocument<200> ver;
        DeserializationError err = deserializeJson(ver, payload);
        if( err ) {
            Log.error(F("WIFI: Failed to parse json" CR));
        } else {
#if LOG_LEVEL==6
            Log.verbose(F("WIFI: Project %s version %s." CR), ver["project"].as<char*>(), ver["version"].as<char*>());
#endif
            int  newVer[3];
            int  curVer[3];
 
            if( parseFirmwareVersionString( newVer, (const char*) ver["version"] ) ) {
                if( parseFirmwareVersionString( curVer, CFG_APPVER) ) {
#if LOG_LEVEL==6
                  Log.verbose(F("OTA : Checking New=%d.%d.%d Cur=%d.%d.%d" CR), newVer[0], newVer[1], newVer[2], curVer[0], curVer[1], curVer[2] );
#endif
                    // Compare major version
                    if( newVer[0] > curVer[0] )
                        newFirmware = true;

                    // Compare minor version
                    if( newVer[0] == curVer[0] && newVer[1] > curVer[1] )
                        newFirmware = true;

                    // Compare patch version
                    if( newVer[0] == curVer[0] && newVer[1] == curVer[1] && newVer[2] > curVer[2] )
                        newFirmware = true;
                }
            }
        }
    } else {
        Log.error(F("WIFI: HTTP Response code %d" CR), httpResponseCode);
    }

    http.end();
//#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Found new version %s." CR), newFirmware?"true":"false");
//#endif
    return newFirmware;
}

//
// Parse a version string in the format M.m.p (eg. 1.2.10)
//
bool Wifi::parseFirmwareVersionString( int (&num)[3], const char *version ) {
//#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Parsing version number %s." CR), version);
//#endif
    char temp[80]; 
    char *s;
    char *p = &temp[0];
    int  i = 0;

    strcpy( &temp[0], version );

    // TODO: Do some error checking on the string, lenght etc. 

    while ((s = strtok_r(p, ".", &p)) != NULL) {
        num[i++] = atoi( s );
    }        

    return true;
}

#endif  // ACTIVATE_OTA && ACTIVATE_WIFI

// EOF 