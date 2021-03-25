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
    setTempFormat('C');
    setPushInterval(30);             
    saveNeeded = false;
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Creating hostname %s." CR), mDNS);
#endif
}

//
// Populate the json document
//
void Config::createJson(DynamicJsonDocument& doc) {
    doc[ CFG_PARAM_MDNS ]              = getMDNS();
    doc[ CFG_PARAM_ID ]                = getID();
    doc[ CFG_PARAM_OTA ]               = getOtaURL();
    doc[ CFG_PARAM_TEMP_FORMAT ]       = String( getTempFormat() );
    doc[ CFG_PARAM_TEMP_ADJ ]          = getTempSensorAdj();
    doc[ CFG_PARAM_PUSH_HTTP ]         = getHttpPushTarget();
    doc[ CFG_PARAM_PUSH_INTERVAL ]     = getPushInterval();
    doc[ CFG_PARAM_PUSH_BLYNK_SERVER ] = getBlynkServer();
    doc[ CFG_PARAM_PUSH_BLYNK_TOKEN ]  = getBlynkToken();
    doc[ CFG_PARAM_PUSH_BLYNK_PORT ]   = getBlynkServerPort();
}

//
// Save json document to file
//
bool Config::saveFile() {
    if( !saveNeeded ) {
#if LOG_LEVEL==6
        Log.verbose(F("CFG : Skipping save, not needed." CR));
#endif
        return true;
    }

#if LOG_LEVEL==6
    Log.verbose(F("CFG : Saving configuration to file." CR));
#endif    

    File configFile = LittleFS.open(CFG_FILENAME, "w");

    if (!configFile) {
        Log.error(F("CFG : Failed to open file " CFG_FILENAME " for save." CR));
        return false;
    }

    DynamicJsonDocument doc(CFG_JSON_BUFSIZE);
    createJson( doc );
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
    Serial.print( CR );
#endif    
    serializeJson(doc, configFile);
    configFile.flush();
    configFile.close();
    saveNeeded = false;
    myConfig.debug();
    Log.notice(F("CFG : Configuration saved to " CFG_FILENAME "." CR));
    return true;}

//
// Load config file from disk
//
bool Config::loadFile() {
#if LOG_LEVEL==6
    Log.verbose(F("CFG : Loading configuration from file." CR));
#endif 

    if (!LittleFS.exists(CFG_FILENAME)) {
        Log.error(F("CFG : Configuration file does not exist " CFG_FILENAME "." CR));
        return false;
    }

    File configFile = LittleFS.open(CFG_FILENAME, "r");

    if (!configFile) {
        Log.error(F("CFG : Failed to open " CFG_FILENAME "." CR));
        return false;
    }

    DynamicJsonDocument doc(CFG_JSON_BUFSIZE);
    DeserializationError err = deserializeJson(doc, configFile);
#if LOG_LEVEL==6
    serializeJson(doc, Serial);
    Serial.print( CR );
#endif    
    configFile.close();

    if( err ) {
        Log.error(F("CFG : Failed to parse " CFG_FILENAME " file, Err: %s, %d." CR), err.c_str(), doc.capacity());
        return false;
    }

#if LOG_LEVEL==6
    Log.verbose(F("CFG : Parsed configuration file." CR));
#endif                    
    if( !doc[ CFG_PARAM_OTA ].isNull() )
        setOtaURL( doc[ CFG_PARAM_OTA ] );
    if( !doc[ CFG_PARAM_MDNS ].isNull() )
        setMDNS( doc[ CFG_PARAM_MDNS ] );
    if( !doc[ CFG_PARAM_TEMP_FORMAT ].isNull() ) {
        String s = doc[ CFG_PARAM_TEMP_FORMAT ];
        setTempFormat( s.charAt(0) );
    }
    if( !doc[ CFG_PARAM_PUSH_HTTP ].isNull() )
        setHttpPushTarget( doc[ CFG_PARAM_PUSH_HTTP ] );
    if( !doc[ CFG_PARAM_PUSH_INTERVAL ].isNull() )
        setPushInterval( doc[ CFG_PARAM_PUSH_INTERVAL ].as<int>() );
    if( !doc[ CFG_PARAM_PUSH_BLYNK_SERVER ].isNull() )
        setBlynkServer( doc[ CFG_PARAM_PUSH_BLYNK_SERVER ] );
    if( !doc[ CFG_PARAM_PUSH_BLYNK_PORT ].isNull() )
        setBlynkServerPort( doc[ CFG_PARAM_PUSH_BLYNK_PORT ].as<int>() );
    if( !doc[ CFG_PARAM_PUSH_BLYNK_TOKEN ].isNull() )
        setBlynkToken( doc[ CFG_PARAM_PUSH_BLYNK_TOKEN ] );
    if( !doc[ CFG_PARAM_TEMP_ADJ ].isNull() )
        setTempSensorAdj( doc[ CFG_PARAM_TEMP_ADJ ].as<float>() );

    myConfig.debug();
    saveNeeded = false;     // Reset save flag 
    Log.notice(F("CFG : Configuration file " CFG_FILENAME " loaded." CR));
    return true;
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
    Log.verbose(F("CFG : Dumping configration " CFG_FILENAME "." CR));
    Log.verbose(F("CFG : ID; '%s'." CR), getID());
    Log.verbose(F("CFG : mDNS; '%s'." CR), getMDNS() );
    Log.verbose(F("CFG : OTA; '%s'." CR), getOtaURL() );
    Log.verbose(F("CFG : Tempformat; '%c'." CR), getTempFormat() );
    Log.verbose(F("CFG : Push http; '%s'." CR), getHttpPushTarget() );
    Log.verbose(F("CFG : Push interval; %d." CR), getPushInterval() );
    Log.verbose(F("CFG : Blynk; '%s' %d '%s'." CR), getBlynkServer(), getBlynkServerPort(), getBlynkToken() );
#endif    
}

// EOF 