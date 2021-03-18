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

Config myConfig;

//
// Create the config class with default settings
//
Config::Config() {
    sprintf(&id[0], "%6x", (unsigned int) ESP.getChipId() );
    sprintf(&mDNS[0], "" WIFI_MDNS "%s", getID() );
    setTempFormat("C");
    setPushInterval("900");             // 15 minutes
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Creating hostname %s." CR), mDNS);
#endif
}

//
// Populate the json document
//
void Config::createJson(DynamicJsonDocument& doc) {
    doc[ CFG_PARAM_MDNS ]             = getMDNS();
    doc[ CFG_PARAM_ID ]               = getID();
    doc[ CFG_PARAM_OTA ]              = getOtaURL();
    doc[ CFG_PARAM_TEMPFORMAT ]       = getTempFormat();
    doc[ CFG_PARAM_PUSH_HTTP ]        = getHttpPushTarget();
    doc[ CFG_PARAM_PUSH_INTERVAL ]    = getPushInterval();
    doc[ CFG_PARAM_PUSH_BLYNKSERVER ] = getBlynkServer();
    doc[ CFG_PARAM_PUSH_BLYNKTOKEN ]  = getBlynkToken();
    doc[ CFG_PARAM_PUSH_BLYNKPORT ]   = getBlynkPort();
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
        LittleFS.begin();

        File configFile = LittleFS.open(CFG_FILENAME, "w");

        if (!configFile) {
            Log.error(F("CFG : Failed to save " CFG_FILENAME "." CR));
        }

        DynamicJsonDocument doc(256);
        createJson( doc );
#if LOG_LEVEL==6
        serializeJson(doc, Serial);
#endif    
        serializeJson(doc, configFile);
        configFile.close();

        saveNeeded = false;
        success = true;
        myConfig.debug();

#if LOG_LEVEL==6
        Log.verbose(F("CFG : Configuration saved to " CFG_FILENAME "." CR));
#endif    

        LittleFS.end();
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
        if (LittleFS.exists(CFG_FILENAME)) {

            File configFile = LittleFS.open(CFG_FILENAME, "r");
            
            if (configFile) {
#if LOG_LEVEL==6
                Log.verbose(F("CFG : Reading configuration file." CR));
#endif    
                char buf[1024];
                size_t size = configFile.size();
                configFile.readBytes(&buf[0], size);

                DynamicJsonDocument cfg(512);
                DeserializationError err = deserializeJson(cfg, buf);

                if( err ) {
                    Log.error(F("CFG : Failed to load " CFG_FILENAME " file." CR));
                } else {
#if LOG_LEVEL==6
                    Log.verbose(F("CFG : Parsed configuration file." CR));
#endif    
                    // If we add new parameters we need to check if they exist in the json file...
                
                    if( !cfg[ CFG_PARAM_OTA ].isNull() )
                        setOtaURL( cfg[ CFG_PARAM_OTA ] );

                    if( !cfg[ CFG_PARAM_MDNS ].isNull() )
                        setMDNS( cfg[ CFG_PARAM_MDNS ] );

                    if( !cfg[ CFG_PARAM_TEMPFORMAT ].isNull() )
                        setTempFormat( cfg[ CFG_PARAM_TEMPFORMAT ] );

                   if( !cfg[ CFG_PARAM_PUSH_HTTP ].isNull() )
                        setHttpPushTarget( cfg[ CFG_PARAM_PUSH_HTTP ] );

                   if( !cfg[ CFG_PARAM_PUSH_INTERVAL ].isNull() )
                        setPushInterval( cfg[ CFG_PARAM_PUSH_INTERVAL ] );

                   if( !cfg[ CFG_PARAM_PUSH_BLYNKSERVER ].isNull() )
                        setBlynkServer( cfg[ CFG_PARAM_PUSH_BLYNKSERVER ] );

                   if( !cfg[ CFG_PARAM_PUSH_BLYNKPORT ].isNull() )
                        setBlynkPort( cfg[ CFG_PARAM_PUSH_BLYNKPORT ] );

                   if( !cfg[ CFG_PARAM_PUSH_BLYNKTOKEN ].isNull() )
                        setBlynkToken( cfg[ CFG_PARAM_PUSH_BLYNKTOKEN ] );

                    myConfig.debug();
                    success = true;
                }
            } else {
                Log.error(F("CFG : Failed to open " CFG_FILENAME "." CR));
            }
        }  else {
            Log.error(F("CFG : Configuration file does not exist " CFG_FILENAME "." CR));
        }

        LittleFS.end();
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
        LittleFS.end();    
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
    Log.verbose(F("CFG : Dumping configration " CFG_FILENAME "." CR));
    Log.verbose(F("CFG : ID; '%s'." CR), getID());
    Log.verbose(F("CFG : mDNS; '%s'." CR), getMDNS() );
    Log.verbose(F("CFG : OTA; '%s'." CR), getOtaURL() );
    Log.verbose(F("CFG : Temp; '%s'." CR), getTempFormat() );
    Log.verbose(F("CFG : Push http; '%s'." CR), getHttpPushTarget() );
    Log.verbose(F("CFG : Push interval; '%s'." CR), getPushInterval() );
    Log.verbose(F("CFG : Blynk; '%s' '%s' '%s'." CR), getBlynkServer(), getBlynkPort(), getBlynkToken() );
#endif    
}

// EOF 