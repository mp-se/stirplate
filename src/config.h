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
#define CFG_APPNAME         "Stir Plate"    // Name of firmware
#define CFG_APPVER          "0.6.0"         // Version of firmware (used wit OTA update to check for newer)
#define CFG_FILENAME        "/config.json"  // Filename of config file

#define WIFI_DEFAULT_SSID   "StirPlate"     // Name of created SSID
#define WIFI_DEFAULT_PWD    "password"      // Password for created SSID
#define WIFI_MDNS           "stirplate"     // Prefix for mdns name
#define WIFI_PORTAL_TIMEOUT 120             // Number of seconds until the config portal is closed

#define POT_MAX_READING     910             // You might need to change this value if the percentage never reach 100%

#define CFG_PARAM_ID                 "id"
#define CFG_PARAM_MDNS               "mdns"
#define CFG_PARAM_OTA                "otaurl"
#define CFG_PARAM_PUSH_BLYNKSERVER   "blynkserver"
#define CFG_PARAM_PUSH_BLYNKPORT     "blynkserverport"
#define CFG_PARAM_PUSH_BLYNKTOKEN    "blynktoken"
#define CFG_PARAM_PUSH_HTTP          "httppush"
#define CFG_PARAM_PUSH_INTERVAL      "pushinterval"
#define CFG_PARAM_TEMPFORMAT         "tempformat"

class Config {
    private:
        char id[10];
        char mDNS[40]; 
        char otaURL[200];
        char tempFormat[2];              // Should be C or F
        char blynkServer[100];
        char blynkToken[100];
        char blynkServerPort[5];
        char httpPushTarget[100];
        char pushInterval[10];

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

        const char*  getPushInterval() { return &pushInterval[0]; }
        void         setPushInterval( const char* s ) { strncpy(&pushInterval[0], s, sizeof(pushInterval)-1); saveNeeded = true; }
        int          getPushIntervalAsInt() { return atoi(&pushInterval[0]); }

        const char*  getBlynkServer() { return &blynkServer[0]; }
        void         setBlynkServer( const char* s ) { strncpy(&blynkServer[0], s, sizeof(blynkServer)-1); saveNeeded = true; }
        const char*  getBlynkToken() { return &blynkToken[0]; }
        void         setBlynkToken( const char* s ) { strncpy(&blynkToken[0], s, sizeof(blynkToken)-1); saveNeeded = true; }
        const char*  getBlynkPort() { return &blynkServerPort[0]; }
        void         setBlynkPort( const char* s ) { strncpy(&blynkServerPort[0], s, sizeof(blynkServerPort)-1); saveNeeded = true; }
        int          getBlynkPortAsInt() { return atoi(&blynkServerPort[0]); }
        bool         isBlynkActive() { return strlen(blynkToken)>0?true:false; }

        const char*  getTempFormat() { return &tempFormat[0]; }
        void         setTempFormat( const char* s ) { strncpy( &tempFormat[0], s, sizeof(tempFormat)-1); saveNeeded = true; }
        bool         isTempC() { return strcmp( &tempFormat[0], "C")?false:true; };
        bool         isTempF() { return strcmp( &tempFormat[0], "F")?false:true; };

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