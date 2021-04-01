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
#include "pwmfan.h"
#include "tempsensor.h"
#include <incbin.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiManager.h>   
#include <LittleFS.h>
Wifi myWifi;
WiFiManager myWifiManager; 
ESP8266WebServer server(80);
bool shouldSaveConfig = false;

// Binary resouces
#if defined( EMBED_HTML )
INCBIN_EXTERN(IndexHtm);
INCBIN_EXTERN(DeviceHtm);
INCBIN_EXTERN(ConfigHtm);
INCBIN_EXTERN(AboutHtm);
#endif

//
// Callback from wifiManager when settings have changed.
//
void saveConfigCallback() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: wifiMgr callback to save params." CR));
#endif
    shouldSaveConfig = true;
}

//
// Callback from webServer when / has been accessed.
//
void webHandleConfig() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /api/config." CR));
#endif
    DynamicJsonDocument doc(CFG_JSON_BUFSIZE);
    myConfig.createJson( doc );
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
    Serial.printf( CR );
#endif    
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out.c_str() );
}

//
// Callback from webServer when / has been accessed.
//
void webHandleReset() {
    String id    = server.arg("id");
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /reset." CR));
#endif
    if( !id.compareTo( myConfig.getID() ) ) {
        server.send(200, "text/plain", "Doing reset...");
        delay(1000);
        ESP.reset();
    } else {
        server.send(400, "text/plain", "Unknown ID.");
    }
}

//
// Get current status from device
//
void webHandleStatus() {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /api/status." CR));
#endif
    DynamicJsonDocument doc(200);
    doc[ CFG_PARAM_RPM ]          = reduceFloatPrecision( myFan.getCurrentRPM() );
    doc[ CFG_PARAM_POWER ]        = reduceFloatPrecision( myFan.getCurrentPower() );
    doc[ CFG_PARAM_TEMP_FORMAT ]  = String( myConfig.getTempFormat() );
    doc[ CFG_PARAM_TEMP_C ]       = reduceFloatPrecision( myTempSensor.getValueCelcius() );
    doc[ CFG_PARAM_TEMP_F ]       = reduceFloatPrecision( myTempSensor.getValueFarenheight() );
    doc[ CFG_PARAM_RSSI ]         = WiFi.RSSI(); 
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
    Serial.printf( CR );
#endif    
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out.c_str() );
}

//
// Callback from webServer when / has been accessed.
//
void webHandleClearWIFI() {
    String id    = server.arg("id");
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: webServer callback for /api/clearwifi." CR));
#endif
    if( !id.compareTo( myConfig.getID() ) ) {
        server.send(200, "text/plain", "Clearing WIFI credentials and doing reset...");
        delay(1000);
        WiFi.disconnect();  // Clear credentials
        ESP.reset();
    } else {
        server.send(400, "text/plain", "Unknown ID.");
    }
}

//
// Callback from webServer when / has been accessed.
//
void webHandleDevice() {
#if LOG_LEVEL==6
    Log.verbose(F("WEB : webServer callback for /api/config." CR));
#endif
    DynamicJsonDocument doc(100);
    doc[ CFG_PARAM_ID ]       = myConfig.getID();
    doc[ CFG_PARAM_APP_NAME ] = CFG_APPNAME;
    doc[ CFG_PARAM_APP_VER ]  = CFG_APPVER;
    doc[ CFG_PARAM_MDNS ]     = myConfig.getMDNS();
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
    Serial.print( CR );
#endif    
    String out;
    serializeJson(doc, out);
    server.send(200, "application/json", out.c_str() );
}

//
// Update device settings.
//
void webHandleConfigDevice() {
    String id = server.arg( CFG_PARAM_ID );
#if LOG_LEVEL==6
    Log.verbose(F("WEB : webServer callback for /api/config/device." CR) );
#endif
    if( !id.equalsIgnoreCase( myConfig.getID() ) ) {
        Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(), myConfig.getID());
        server.send(400, "text/plain", "Invalid ID.");
        return;
    }
#if LOG_LEVEL==6
    Log.verbose(F("WEB : mdns=%s, temp-format=%s." CR), server.arg( CFG_PARAM_MDNS ).c_str(), server.arg( CFG_PARAM_TEMP_FORMAT ).c_str() );
#endif
    myConfig.setMDNS( server.arg( CFG_PARAM_MDNS ).c_str() );
    myConfig.setTempFormat( server.arg( CFG_PARAM_TEMP_FORMAT ).charAt(0) );
    myConfig.saveFile();
    server.sendHeader("Location", "/config.htm#collapseOne", true);  
    server.send(302, "text/plain", "Device config updated" );
}

//
// Update push settings.
//
void webHandleConfigPush() {
    String id = server.arg( CFG_PARAM_ID );
#if LOG_LEVEL==6
    Log.verbose(F("WEB : webServer callback for /api/config/push." CR) );
#endif
    if( !id.equalsIgnoreCase( myConfig.getID() ) ) {
        Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(), myConfig.getID());
        server.send(400, "text/plain", "Invalid ID.");
        return;
    }
#if LOG_LEVEL==6
    Log.verbose(F("WEB : http=%s interval=%s." CR), server.arg( CFG_PARAM_PUSH_HTTP ).c_str(), server.arg( CFG_PARAM_PUSH_INTERVAL ).c_str() );
#endif
    myConfig.setHttpPushTarget( server.arg( CFG_PARAM_PUSH_HTTP ).c_str() );
    myConfig.setPushInterval( server.arg( CFG_PARAM_PUSH_INTERVAL ).c_str() );
    myConfig.saveFile();
    server.sendHeader("Location", "/config.htm#collapseTwo", true);  
    server.send(302, "text/plain", "Push config updated" );
}

//
// Update gravity settings.
//
void webHandleConfigBlynk() {
    String id = server.arg( CFG_PARAM_ID );
#if LOG_LEVEL==6
    Log.verbose(F("WEB : webServer callback for /api/config/blynk." CR) );
#endif
    if( !id.equalsIgnoreCase( myConfig.getID() ) ) {
        Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(), myConfig.getID());
        server.send(400, "text/plain", "Invalid ID.");
        return;
    }
#if LOG_LEVEL==6
    Log.verbose(F("WEB : blynk server=%s, token=%s, port=%d." CR), server.arg( CFG_PARAM_PUSH_BLYNK_SERVER ).c_str(), server.arg( CFG_PARAM_PUSH_BLYNK_TOKEN ).c_str(), server.arg( CFG_PARAM_PUSH_BLYNK_PORT ).c_str());
#endif
    myConfig.setBlynkServer( server.arg( CFG_PARAM_PUSH_BLYNK_SERVER ).c_str() );
    myConfig.setBlynkToken( server.arg( CFG_PARAM_PUSH_BLYNK_TOKEN ).c_str() ); 
    myConfig.setBlynkServerPort( server.arg( CFG_PARAM_PUSH_BLYNK_PORT ).toInt() ); 
    myConfig.saveFile();
    server.sendHeader("Location", "/config.htm#collapseThree", true);  
    server.send(302, "text/plain", "Gravity config updated" );
}

//
// Update hardware settings.
//
void webHandleConfigHardware() {
    String id = server.arg( CFG_PARAM_ID );
#if LOG_LEVEL==6
    Log.verbose(F("WEB : webServer callback for /api/config/hardware." CR) );
#endif
    if( !id.equalsIgnoreCase( myConfig.getID() ) ) {
        Log.error(F("WEB : Wrong ID received %s, expected %s" CR), id.c_str(), myConfig.getID());
        server.send(400, "text/plain", "Invalid ID.");
        return;
    }
#if LOG_LEVEL==6
    Log.verbose(F("WEB : tempadj=%s, ota=%s." CR), server.arg( CFG_PARAM_TEMP_ADJ ).c_str(), server.arg( CFG_PARAM_OTA ).c_str() );
#endif
    myConfig.setTempSensorAdj( server.arg( CFG_PARAM_TEMP_ADJ ).toFloat() );
    myConfig.setOtaURL( server.arg( CFG_PARAM_OTA ).c_str() ); 
    myConfig.saveFile();
    server.sendHeader("Location", "/config.htm#collapseFour", true);  
    server.send(302, "text/plain", "Hardware config updated" );
}

//
// Setup the Web Server callbacks and start it
//
void Wifi::setupWebServer() {
#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Setting up web server." CR));
#endif

    // Static content
#if defined( EMBED_HTML )
    server.on("/",[]() {
        server.send_P(200, "text/html", (const char*) gIndexHtmData, gIndexHtmSize );
    } );
    server.on("/index.htm",[]() {
        server.send_P(200, "text/html", (const char*) gIndexHtmData, gIndexHtmSize );
    } );
    server.on("/device.htm",[]() {
        server.send_P(200, "text/html", (const char*) gDeviceHtmData, gDeviceHtmSize );
    } );
    server.on("/config.htm",[]() {
        server.send_P(200, "text/html", (const char*) gConfigHtmData, gConfigHtmSize );
    } );
    server.on("/about.htm",[]() {
        server.send_P(200, "text/html", (const char*) gAboutHtmData, gAboutHtmSize );
    } );
#else
    // Show files in the filessytem at startup
    FSInfo fs;
    LittleFS.info(fs);
    Log.notice( F("File system: Total=%d, Used=%d." CR), fs.totalBytes, fs.usedBytes );
    Dir dir = LittleFS.openDir("/");
    while( dir.next() ) {
        Log.notice( F("File: %s, %d bytes" CR), dir.fileName().c_str(), dir.fileSize() );
    }

    server.serveStatic("/", LittleFS, "/index.htm" );
    server.serveStatic("/index.htm", LittleFS, "/index.htm" );
    server.serveStatic("/device.htm", LittleFS, "/device.htm" );
    server.serveStatic("/config.htm", LittleFS, "/config.htm" );
    server.serveStatic("/about.htm", LittleFS, "/about.htm" );
#endif

    server.on("/api/config", webHandleConfig);
    server.on("/api/device", webHandleDevice);
    server.on("/api/config/device", webHandleConfigDevice);
    server.on("/api/config/blynk", webHandleConfigBlynk);
    server.on("/api/config/push", webHandleConfigPush);
    server.on("/api/config/hardware", webHandleConfigHardware);
    server.on("/api/status", webHandleStatus);
    server.on("/api/reset", webHandleReset);
    server.on("/api/clearwifi", webHandleClearWIFI);

    server.begin();
}

//
// Connect to last known access point or create one if connection is not working. 
//
bool Wifi::connect( bool showPortal ) {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connecting to WIFI via connection manager (portal=%s)." CR), showPortal?"true":"false");
    myWifiManager.setDebugOutput(true);    
#endif
    unsigned long startMillis = millis();

    // Connect to WIFI
    if( showPortal ) {
        Log.notice(F("WIFI: Starting wifi portal." CR));
        myWifiManager.setBreakAfterConfig( true );
        myWifiManager.setSaveConfigCallback(saveConfigCallback);
        myWifiManager.setMinimumSignalQuality(10);  
        myWifiManager.setClass("invert");
        myWifiManager.setHostname( myConfig.getMDNS() );

        WiFiManagerParameter mdnsParam("mDNS", "hostname", myConfig.getMDNS(), 20);
        myWifiManager.addParameter( &mdnsParam );

        myWifiManager.startConfigPortal( WIFI_DEFAULT_SSID, WIFI_DEFAULT_PWD );

        if( shouldSaveConfig ) {
            myConfig.setMDNS( mdnsParam.getValue() );
            myConfig.saveFile();
        }
    }

    // Connect to wifi
    int i = 0;

    WiFi.begin();
    while( WiFi.status() != WL_CONNECTED ) {
        delay(100);
        Serial.print( "." );

        if( i++ > 100 ) {
            LittleFS.end();
            ESP.reset();
        }
    }
    Serial.print( CR );
    connectedFlag = true;

    Log.notice( F("WIFI: IP=%s, Connect time %d s" CR), WiFi.localIP().toString().c_str(), abs(millis() - startMillis)/1000);

#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Connect returned %s." CR), connectedFlag?"True":"False" );
#endif

    if( connectedFlag ) {
#if LOG_LEVEL==6
        Log.verbose(F("WIFI: Starting mDNS for %s." CR), myConfig.getMDNS() );
#endif
        MDNS.begin( myConfig.getMDNS() );
        MDNS.addService("http", "tcp", 80);
        setupWebServer();
    }

    return connectedFlag;
}

//
// called from main loop
//
void Wifi::loop() {
    // Dont put serial debug output in this call
    server.handleClient();
    MDNS.update();
}

//
// This will erase the stored credentials and forcing the WIFI manager to AP mode.
//
bool Wifi::disconnect() {
    Log.notice(F("WIFI: Erasing stored WIFI credentials." CR));
    // Erase WIFI credentials
    return WiFi.disconnect(true);
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

#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Updating firmware." CR));
#endif

    WiFiClient client;
    String serverPath = myConfig.getOtaURL();
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

    if( !myConfig.isOtaActive() ) {
        Log.notice(F("WIFI: No OTA URL defined, skipping." CR));
        return false;
    }

#if LOG_LEVEL==6
//    Log.verbose(F("WIFI: Checking if new version exist." CR));
#endif
    WiFiClient client;
    HTTPClient http;
    String serverPath = myConfig.getOtaURL();
    serverPath += "version.json";

    // Your Domain name with URL path or IP address with path
    http.begin( client, serverPath);
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
      
    if (httpResponseCode==200) {
        Log.notice(F("WIFI: HTTP Response code %d" CR), httpResponseCode);
 
        String payload = http.getString();
#if LOG_LEVEL==6
        Log.verbose(F("WIFI: Payload %s." CR), payload.c_str());
#endif
        DynamicJsonDocument ver(100);
        DeserializationError err = deserializeJson(ver, payload);
        if( err ) {
            Log.error(F("WIFI: Failed to parse json %s" CR), err);
        } else {
#if LOG_LEVEL==6
            Log.verbose(F("WIFI: Project %s version %s." CR), (const char*) ver["project"], (const char*) ver["version"]);
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
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Found new version %s." CR), newFirmware?"true":"false");
#endif
    return newFirmware;
}

//
// Parse a version string in the format M.m.p (eg. 1.2.10)
//
bool Wifi::parseFirmwareVersionString( int (&num)[3], const char *version ) {
#if LOG_LEVEL==6
    Log.verbose(F("WIFI: Parsing version number %s." CR), version);
#endif
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