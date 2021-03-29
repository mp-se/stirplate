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
#ifndef _CONFIG_H
#define _CONFIG_H

// Includes
#include <Arduino.h>
#include <ArduinoJson.h>

// Defintions
#define CFG_APPNAME         "Stir Plate"      // Name of firmware
#define CFG_FILENAME        "/stirplate.json" // Filename of config file

#define WIFI_DEFAULT_SSID   "Stirplate"     // Name of created SSID
#define WIFI_DEFAULT_PWD    "password"      // Password for created SSID
#define WIFI_MDNS           "stirplate"     // Prefix for mdns name
#define WIFI_PORTAL_TIMEOUT 120             // Number of seconds until the config portal is closed

#define POT_MAX_READING     910             // You might need to change this value if the percentage never reach 100%
#define CFG_JSON_BUFSIZE    500

// Configuration params
#define CFG_PARAM_ID                 "id"
#define CFG_PARAM_MDNS               "mdns"
#define CFG_PARAM_OTA                "ota-url"
#define CFG_PARAM_PUSH_BLYNK_SERVER  "blynk-server"
#define CFG_PARAM_PUSH_BLYNK_PORT    "blynk-server-port"
#define CFG_PARAM_PUSH_BLYNK_TOKEN   "blynk-token"
#define CFG_PARAM_PUSH_HTTP          "http-push"
#define CFG_PARAM_PUSH_INTERVAL      "push-interval"
#define CFG_PARAM_TEMP_FORMAT        "temp-format"
#define CFG_PARAM_TEMP_ADJ           "temp-adjustment-value"          // Correction value for temp sensor

// API params
#define CFG_PARAM_RPM                "rpm"
#define CFG_PARAM_POWER              "power"
#define CFG_PARAM_RSSI               "rssi"
#define CFG_PARAM_TEMP_C             "temp-c"
#define CFG_PARAM_TEMP_F             "temp-f"
#define CFG_PARAM_APP_NAME           "app-name"
#define CFG_PARAM_APP_VER            "app-ver"

class Config {
    private:
        String id;
        String mDNS; 
        String otaURL;
        char   tempFormat;                      // C, F
        String blynkServer;
        String blynkToken;
        int    blynkServerPort;
        String httpPushTarget;
        int    pushInterval;
        float  tempSensorAdj;                   // This value will be added to the read sensor value
 
        // Set this flag if config has changed
        bool saveNeeded;

        void formatFileSystem();
        void debug();

    public:
        Config();
        const char*  getID() { return id.c_str(); };

        const char*  getMDNS() { return mDNS.c_str(); }
        void         setMDNS( String s ) { mDNS = s; saveNeeded = true; }

        const char*  getOtaURL() { return otaURL.c_str(); }
        void         setOtaURL( String s ) { otaURL = s; saveNeeded = true; }
        bool         isOtaActive() { return otaURL.length()>0?true:false; }

        const char*  getHttpPushTarget() { return httpPushTarget.c_str(); }
        void         setHttpPushTarget( String s ) { httpPushTarget = s; saveNeeded = true; }
        bool         isHttpActive() { return httpPushTarget.length()>0?true:false; }

        int          getPushInterval() { return pushInterval; }
        void         setPushInterval( int v ) { pushInterval = v; saveNeeded = true; }
        void         setPushInterval( String s ) { pushInterval = s.toInt(); saveNeeded = true; }

        const char*  getBlynkServer() { return blynkServer.c_str(); }
        void         setBlynkServer( String s ) { blynkServer = s;; saveNeeded = true; }

        const char*  getBlynkToken() { return blynkToken.c_str(); }
        void         setBlynkToken( String s ) { blynkToken = s; saveNeeded = true; }
        bool         isBlynkActive() { return blynkToken.length()>0?true:false; }

        int          getBlynkServerPort() { return blynkServerPort; }
        void         setBlynkServerPort( int v ) { blynkServerPort = v; saveNeeded = true; }
        void         setBlynkServerPort( String s ) { blynkServerPort = s.toInt(); saveNeeded = true; }

        char         getTempFormat() { return tempFormat; }
        void         setTempFormat( char c ) { tempFormat = c; saveNeeded = true; }
        bool         isTempC() { return tempFormat=='C'?true:false; };
        bool         isTempF() { return tempFormat=='F'?true:false; };

        float        getTempSensorAdj() { return tempSensorAdj; }
        void         setTempSensorAdj( float f ) { tempSensorAdj = f; saveNeeded = true; }
        void         setTempSensorAdj( String s ) { tempSensorAdj = s.toFloat(); saveNeeded = true; }

        // IO functions
        void createJson(DynamicJsonDocument& doc);
        bool saveFile();
        bool loadFile();
        void checkFileSystem();
        bool isSaveNeeded() { return saveNeeded; };
        void setSaveNeeded() { saveNeeded = true; };
};

// Global instance created
extern Config myConfig;

#endif // _CONFIG_H

// EOF