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
        char  id[10];
        char  mDNS[40]; 
        char  otaURL[100];
        char  tempFormat;                      // C, F
        char  blynkServer[100];
        char  blynkToken[100];
        int   blynkServerPort;
        char  httpPushTarget[100];
        int   pushInterval;
        float tempSensorAdj;                   // This value will be added to the read sensor value
 
        // Set this flag if config has changed
        bool saveNeeded;

        void formatFileSystem();
        void debug();

    public:
        Config();
        const char*  getID() { return &id[0]; };

        const char*  getMDNS() { return &mDNS[0]; }
        void         setMDNS( const char* s ) { strncpy( &mDNS[0], s, sizeof(mDNS)-1); saveNeeded = true; }

        const char*  getOtaURL() { return &otaURL[0]; }
        void         setOtaURL( const char* s ) { strncpy( &otaURL[0], s, sizeof(otaURL)-1); saveNeeded = true; }
        bool         isOtaActive() { return strlen(&otaURL[0])>0?true:false; }

        const char*  getHttpPushTarget() { return &httpPushTarget[0]; }
        void         setHttpPushTarget( const char* s ) { strncpy(&httpPushTarget[0], s, sizeof(httpPushTarget)-1); saveNeeded = true; }
        bool         isHttpActive() { return strlen(&httpPushTarget[0])>0?true:false; }

        int          getPushInterval() { return pushInterval; }
        void         setPushInterval( int v ) { pushInterval = v; saveNeeded = true; }
        void         setPushInterval( const char* s ) { pushInterval = atoi(s); saveNeeded = true; }

        const char*  getBlynkServer() { return &blynkServer[0]; }
        void         setBlynkServer( const char* s ) { strncpy(&blynkServer[0], s, sizeof(blynkServer)-1); saveNeeded = true; }

        const char*  getBlynkToken() { return &blynkToken[0]; }
        void         setBlynkToken( const char* s ) { strncpy(&blynkToken[0], s, sizeof(blynkToken)-1); saveNeeded = true; }
        bool         isBlynkActive() { return strlen(blynkToken)>0?true:false; }

        int          getBlynkServerPort() { return blynkServerPort; }
        void         setBlynkServerPort( int v ) { blynkServerPort = v; saveNeeded = true; }
        void         setBlynkServerPort( const char* s ) { blynkServerPort = atoi(s); saveNeeded = true; }

        char         getTempFormat() { return tempFormat; }
        void         setTempFormat( char c ) { tempFormat = c; saveNeeded = true; }
        bool         isTempC() { return tempFormat=='C'?false:true; };
        bool         isTempF() { return tempFormat=='F'?false:true; };

        float        getTempSensorAdj() { return tempSensorAdj; }
        void         setTempSensorAdj( float f ) { tempSensorAdj = f; saveNeeded = true; }
        void         setTempSensorAdj( const char* s ) { tempSensorAdj = atof(s); saveNeeded = true; }

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