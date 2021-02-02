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
#include "config.h"
#include "helper.h"
#include <LittleFS.h>

Config config;

//
// Create the config class with default settings
//
Config::Config() {
    sprintf(&mDNS[0], "stirplate%6x", (unsigned int) ESP.getChipId() );
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Creating hostname %s." CR), mDNS);
#endif
}

//
// Populate the json document
//
void Config::createJson(StaticJsonDocument<512>& doc) {
    doc["mDNS"]                = mDNS;

    doc["OtaURL"]              = otaUrl;

    doc["BlynkServer"]         = blynkServer;
    doc["BlynkServerPort"]     = blynkServerPort;
    doc["BlynkToken"]          = blynkToken;
}

//
// Save json document to file
//
bool Config::saveFile() {

    bool success = false;

    if( saveNeeded ) {
#if LOG_LEVEL==6
        Log.verbose(F("CFG : Saving configuration." CR));
#endif    

        File configFile = LittleFS.open("/config.json", "w");

        if (!configFile) {
            Log.error(F("CFG : Failed to save /config.json." CR));
        }

        StaticJsonDocument<512> doc;
        createJson( doc );
#if LOG_LEVEL==6
        serializeJson(doc, Serial);
#endif    
        serializeJson(doc, configFile);
        configFile.close();

        saveNeeded = false;
        success = true;
        config.debug();

#if LOG_LEVEL==6
        Log.verbose(F("CFG : Configuration saved to /config.json." CR));
#endif    
    } else {
#if LOG_LEVEL==6
        Log.verbose(F("CFG : Skipping save." CR));
#endif    
        success = true;
    }

    return success;
}

//
// Load config file from disk
//
bool Config::loadFile() {

    bool success = false;

#if LOG_LEVEL==6
    Log.verbose(F("CFG : Loading configuration file." CR));
#endif    

    if (LittleFS.begin()) {
#if LOG_LEVEL==6
        Log.verbose(F("CFG : Filesystem mounted." CR));
#endif    
        if (LittleFS.exists("/config.json")) {

            File configFile = LittleFS.open("/config.json", "r");
            
            if (configFile) {
#if LOG_LEVEL==6
                Log.verbose(F("CFG : Reading configuration file." CR));
#endif    
                char buf[1024];
                size_t size = configFile.size();
                configFile.readBytes(&buf[0], size);

                StaticJsonDocument<1024> cfg;
                DeserializationError err = deserializeJson(cfg, buf);

                if( err ) {
                    Log.error(F("CFG : Failed to load /config.json file." CR));
                } else {
#if LOG_LEVEL==6
                    Log.verbose(F("CFG : Parsed configuration file." CR));
#endif    
                    // If we add new parameters we need to check if they exist in the json file...
                    //if( !cfg["OtaURL"].isNull() )

                    strcpy( otaUrl, cfg["OtaURL"]);

                    strcpy( blynkServer, cfg["BlynkServer"]);
                    strcpy( blynkServerPort, cfg["BlynkServerPort"]);
                    strcpy( blynkToken, cfg["BlynkToken"] );

                    config.debug();
                    success = true;
                }
            } else {
                Log.error(F("CFG : Failed to open /config.json." CR));
            }
        }  else {
            Log.error(F("CFG : Configuration file does not exist /config.json." CR));
        }
    } else {
        Log.error(F("CFG : Failed to mount file system." CR));
    }

    return success;
}

//
// Check if file system can be mounted, if not we format it.
//
void Config::formatFileSystem() {
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Formating filesystem." CR));
#endif    
    LittleFS.format();
}

//
// Check if file system can be mounted, if not we format it.
//
void Config::checkFileSystem() {

#if LOG_LEVEL==6
    Log.verbose(F("CFG : Checking if filesystem is valid." CR));
#endif    

    if (LittleFS.begin()) {
#if LOG_LEVEL==6
        Log.verbose(F("CFG : Filesystem mounted." CR));
#endif    
    } else {
        Log.error(F("CFG : Unable to mount file system, formatting..." CR));
        LittleFS.format();
    }
}

//
// Dump the configuration to the serial port
//
void Config::debug() {
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Dumping configration." CR));
    Log.verbose(F("CFG : OTA; '%s'." CR), otaUrl );
    Log.verbose(F("CFG : Blynk; '%s' '%s' '%s'." CR), blynkServer, blynkServerPort, blynkToken );
#endif    
}

// EOF 