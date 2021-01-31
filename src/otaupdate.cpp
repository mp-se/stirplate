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
#include "otaupdate.h"

#ifdef ACTIVATE_OTA

//
//
//
bool OtaUpdate::updateFirmware() {

    if( !newVersion ) {
        Log.notice(F("OTA : No newer version exist, skipping update." CR));
        return false;
    }

#if LOG_LEVEL==6
    Log.verbose(F("OTA : Updating firmware." CR));
#endif

    WiFiClient client;
    String serverPath = OTA_HTTP;
    serverPath += "firmware.bin";

    // TODO: Update code to work with https connection (currently not using that at home)

    HTTPUpdateResult ret = ESPhttpUpdate.update(client, serverPath);

    switch(ret) {
        case HTTP_UPDATE_FAILED:
            Log.error(F("OTA : Updating failed %d, %s." CR), ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
            break;

        case HTTP_UPDATE_NO_UPDATES:
            break;

        case HTTP_UPDATE_OK:
            Log.notice("OTA : Updated succesfull, rebooting." );
            ESP.reset();
            break;
    }

    return false;
}

//
//
//
bool OtaUpdate::checkVersion() {
#if LOG_LEVEL==6
    Log.verbose(F("OTA : Checking if new version exist." CR));
#endif

    WiFiClient client;
    HTTPClient http;
    String serverPath = OTA_HTTP;
    serverPath += "version.json";

    // Your Domain name with URL path or IP address with path
    http.begin( client, serverPath);
    
    // Send HTTP GET request
    int httpResponseCode = http.GET();
      
    if (httpResponseCode==200) {
        Log.notice(F("OTA : HTTP Response code %d" CR), httpResponseCode);
 
        String payload = http.getString();
#if LOG_LEVEL==6
        Log.verbose(F("OTA : Payload %s." CR), payload.c_str());
#endif
        StaticJsonDocument<200> ver;
        DeserializationError err = deserializeJson(ver, payload);
        if( err ) {
            Log.error(F("OTA : Failed to parse json" CR));
        } else {
#if LOG_LEVEL==6
            Log.verbose(F("OTA : Project %s version %s." CR), ver["project"].as<char*>(), ver["version"].as<char*>());
#endif
            int  newVer[3];
            int  curVer[3];
 
            if( parseVersionString( newVer, (const char*) ver["version"] ) ) {
                if( parseVersionString( curVer, CFG_APPVER) ) {
#if LOG_LEVEL==6
//                  Log.verbose(F("OTA : Checking New=%d.%d.%d Cur=%d.%d.%d" CR), newVer[0], newVer[1], newVer[2], curVer[0], curVer[1], curVer[2] );
#endif
                    // Compare major version
                    if( newVer[0] > curVer[0] )
                        newVersion = true;

                    // Compare minor version
                    if( newVer[0] == curVer[0] && newVer[1] > curVer[1] )
                        newVersion = true;

                    // Compare patch version
                    if( newVer[0] == curVer[0] && newVer[1] == curVer[1] && newVer[2] > curVer[2] )
                        newVersion = true;
                }
            }
        }
    } else {
        Log.error(F("OTA : HTTP Response code %d" CR), httpResponseCode);
    }

    http.end();

#if LOG_LEVEL==6
    Log.verbose(F("OTA : Found new version %s." CR), newVersion?"true":"false");
#endif
    return newVersion;
}

//
// Parse a version string in the format M.m.p (eg. 1.2.10)
//
bool OtaUpdate::parseVersionString( int (&num)[3], const char *version ) {
#if LOG_LEVEL==6
    Log.verbose(F("OTA : Parsing version number %s." CR), version);
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

#endif

// EOF 