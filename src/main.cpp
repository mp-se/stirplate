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
#include "helper.h"
#include "display.h"
#include "pwmfan.h"
#include "config.h"
#include "blynk.h"
#include "analogsensor.h"
#include "tempsensor.h"
#include "wifi.h"

/*
 * This is the fan I used for my build.
 * https://noctua.at/pub/media/wysiwyg/Noctua_PWM_specifications_white_paper.pdf
 * Blue = PWM
 * Green = RPM 
 * Yellow = 12V
 * Black = GND
 * 
 * PWM Frequency: Target frequency 25 kHz, 
 * acceptable operational range 21 kHz to 28 kHz 
 * Maximum voltage for logic low: VIL = 0.8 V 
 * Absolute maximum current sourced: Imax = 5 mA (short circuit current)  
 * Absolute maximum voltage level: VMax = 5.25 V (open circuit voltage) 
 * This signal must be pulled up to a maximum of 5.25V within the fan.
 */

// Define constats for this program
#if LOG_LEVEL==6
const int        interval = 2000;    // ms, time to wait between changes to output (slow down if debugging)
#else
const int        interval = 500;     // ms, time to wait between changes to output
#endif
const static int tachPIN = 12;       // Measure speed on FAN. D6 PIN on ESP-12F
unsigned long    lastMillis = 0;

#if defined( ACTIVATE_BLYNK ) && defined( ACTIVATE_WIFI )
extern BlynkPins blynk;
#endif

//
// Callback for tachimeter
// 
void ICACHE_RAM_ATTR handleTachiometerInterrupt() {
  stirFan.tachCallback();
}

//
// Main setup 
//
void setup() {

  // Wait for the debugger to start
  //delay(10000);

  // Initialize pin outputs
  Log.notice(F("Main: Started setup for %s." CR), String( ESP.getChipId(), HEX).c_str() );
  printBuildOptions();

  Log.notice(F("Main: Loading configuration." CR));
  //config.formatFileSystem();    // Erase the config file
  config.checkFileSystem();
  config.loadFile();

  Log.notice(F("Main: Setting up devices." CR));

#if LOG_LEVEL==6
//  Log.verbose(F("MAIN: Wait 5s for debugger to come online." CR));
//  delay(5000);
#endif

  // Setup watchdog
  ESP.wdtDisable();
  ESP.wdtEnable( interval*2 );

  // Setup display
  Log.notice(F("Main: Looking for display." CR));
  stirDisplay.init();

  char buffer[20];
  sprintf( &buffer[0], "%s", CFG_APPNAME );
  stirDisplay.printText( 0, 0, &buffer[0] );    

#if defined( ACTIVATE_WIFI )
  stirDisplay.printText( 0, 1, "Connect wifi    " );    
  //stirWifi.disconnect();   // clear current wifi settings.
  stirWifi.connect();
  if( stirWifi.isConnected() )
    Log.notice(F("Main: Connected to wifi ip=%s." CR), stirWifi.getIPAddress().c_str() );
#endif

#if defined( ACTIVATE_WIFI ) && defined( ACTIVATE_OTA )
  stirDisplay.printText( 0, 1, "Checking for upd" );    
  if( stirWifi.isConnected() && stirWifi.checkFirmwareVersion() ) {
    delay(500);
    stirDisplay.printText( 0, 1, "Updating        " );    
    stirWifi.updateFirmware();
  }
#endif

  // Setup interrput callback for tachiometer (fan rotation)
  pinMode(tachPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachPIN), handleTachiometerInterrupt, FALLING);

#if defined( ACTIVATE_WIFI ) && defined( ACTIVATE_BLYNK )
  if( stirWifi.isConnected() && config.isBlynkActive() ) {
    Log.notice(F("Main: Connecting to blynk." CR));
    stirDisplay.printText( 0, 1, "Connect blynk   " );   
    blynk.connect( config.blynkToken, config.blynkServer, config.getBlynkPort() );
  }
#endif

#if LOG_LEVEL==6
//  Log.verbose(F("MAIN: Wait 5s." CR));
//  delay(5000);
#endif
  Log.notice(F("Main: Setup is completed." CR));
}

//
// Main loop
//
void loop() {

#if defined( ACTIVATE_WIFI )
  stirWifi.loop();    
#endif
#if defined( ACTIVATE_BLYNK ) && defined( ACTIVATE_WIFI )
  blynk.run();
#endif 

  if( abs(millis() - lastMillis) > interval ) {
    int vin = stirAnalogSensor.readSensor();

    // setPower will map the value to the range supported by the PWM output
    stirFan.setPower( vin, 0, 1024 );   
    int rpm = stirFan.getCurrentRPM();
    int pwr = stirFan.getCurrentPower();

    // Use the lower line to create a power bar that indicate power to stirplate
#if LOG_LEVEL==6
    Log.verbose(F("MAIN: POT = %d, Percentage %d, RPM=%d." CR), vin, pwr, rpm);
#endif

#if defined( ACTIVATE_BLYNK ) && defined( ACTIVATE_WIFI )
  if( stirWifi.isConnected() && config.isBlynkActive() ) {
    blynk.writeRemoteRPM( rpm );
    blynk.writeRemotePower( pwr );
    blynk.writeRemoteVer(CFG_APPVER);
  }
#endif

  // Display Layout 
  //
  //  |123456789.123456| - Startup messages
  // 1|Stir plate      |
  // 2|Connect wifi    | - Step 1 
  // 2|Firmware check  | - Step 2
  // 2|Updating        | - Step 3
  // 2|Connect blynk   | - Step 4

  //  |123456789.123456| - No power on 
  // 1|Stir plate 0.0.0| - Version 
  // 2|wifi            | - wifi connected
  // 2|No wifi         | - no wifi connected

  //  |123456789.123456| - Power on
  // 1|Stir plate  0rpm| - RPM 
  // 2|-progress-  000%| - Progress + % power


    // If there is no power we show the version number, else the RPM
    char s[10];

    if( stirFan.getCurrentPower() < 10 ) {    // Show the version number 
      sprintf( &s[0], "%5s", CFG_APPVER);
#if defined( ACTIVATE_WIFI )      
      if( stirWifi.isConnected() )
        stirDisplay.printText( 0, 1, "WIFI            " );
      else
        stirDisplay.printText( 0, 1, "NO WIFI         " );
#else 
        stirDisplay.printText( 0, 1, "                " );
#endif // ACTIVATE_WIFI
    }
    else {
      stirDisplay.printProgressBar(0,1, pwr);
      sprintf( &s[0], "%5d", stirFan.getCurrentRPM());
    }

    stirDisplay.printText( 11, 0, &s[0] );
    lastMillis = millis();
  }
}

// EOF