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

// defintions
#define CFG_APPNAME         "Stir Plate"    // Name of firmware
#define CFG_APPVER          "0.4.0"         // Version of firmware (used wit OTA update to check for newer)

#define WIFI_DEFAULT_SSID   "StirPlate"     // Name of created SSID
#define WIFI_DEFAULT_PWD    "password"      // Password for created SSID
#define WIFI_PORTAL_TIMEOUT 120             // Number of seconds until the config portal is closed

class Config {
    public:
        // Device settings
        char mDNS[40]; 

        // OTA configuration
        char otaUrl[40];

        // Blynk configuration
        char blynkServer[40];
        char blynkToken[40];
        char blynkServerPort[5];

        // Set this flag if config has changed
        bool saveNeeded = false;

        // Intialize
        Config();

        // Conversion functions
        int  getBlynkPort() { return atoi(&blynkServerPort[0]); }
        bool isBlynkActive() { return strlen(blynkToken)>0?true:false; }
        bool isOtaActive() { return strlen(otaUrl)>0?true:false; }

        // IO functions
        bool saveFile();
        void createJson(StaticJsonDocument<512>& doc);
        bool loadFile();
        void debug();
        void checkFileSystem();
        void formatFileSystem();
};

// Global instance created
extern Config config;

#endif // _CONFIG_H

// EOF