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
#include "main.h"
#include "serial_debug.h"
#include "display.h"
#include "pwmfan.h"
#include "blynk.h"
#include "analogsensor.h"
#include "mysecrets.h"
#include "wifi.h"
#include "otaupdate.h"

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
const int        interval = 500;     // ms, time to wait between changes to output
const static int tachPIN = 12;       // Measure speed on FAN. D6 PIN on ESP-12F

SerialDebug   serial;                // Setting up the serial debug logging (Configure Logging library)
Display       *display = 0;
PwmFan        *fan = 0;
AnalogSensor  *pot = 0;
Wifi          *wifi = 0;

unsigned long lastMillis = 0;

#ifdef ACTIVATE_BLYNK
extern BlynkPins blynk;
#endif

//
// Callback for tachimeter
// 
void ICACHE_RAM_ATTR handleTachiometerInterrupt() {
  if( fan )
    fan->tachCallback();
}

//
// Main setup 
//
void setup() {

  // Initialize pin outputs
  Log.notice(F("Main: Started setup." CR));
  fan = new PwmFan();
  pot = new AnalogSensor();

#if LOG_LEVEL==6
//  Log.verbose(F("MAIN: Wait 5s for debugger to come online." CR));
//  delay(5000);
#endif

  // Setup watchdog
  ESP.wdtDisable();
  ESP.wdtEnable( interval*2 );

  // Setup display
  Log.notice(F("Main: Looking for display." CR));
  display = new Display();

  char buffer[20];
  sprintf( &buffer[0], "%s", CFG_APPNAME );
  display->printText( 0, 0, &buffer[0] );    

  wifi = new Wifi();
  display->printText( 0, 1, "Connect wifi    " );    
  wifi->connect( WIFI_SECRET_AP, WIFI_SECRET_PWD);

  OtaUpdate ota;
  display->printText( 0, 1, "Checking for upd" );    
  if( ota.checkVersion() ) {
    display->printText( 0, 1, "Updating        " );    
    ota.updateFirmware();
  }

  // Setup interrput callback for tachiometer (fan rotation)
  pinMode(tachPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachPIN), handleTachiometerInterrupt, FALLING);

#ifdef ACTIVATE_BLYNK
  Log.notice(F("Main: Connecting to blynk." CR));
  display->printText( 0, 1, "Connect blynk   " );    
  blynk.connect( BLYNK_TOKEN, BLYNK_SERVER, 8080 );
  blynk.writeRemoteVer(CFG_APPVER);
#endif

#if LOG_LEVEL==6
//  Log.verbose(F("MAIN: Wait 5s." CR));
//  delay(5000);
#endif
}

void loop() {

  if( abs(millis() - lastMillis) > interval ) {

#ifdef ACTIVATE_BLYNK
    blynk.run();
#endif

    int vin = pot->readSensor();
    
    // setPower will map the value to the range supported by the PWM output
    fan->setPower( vin, 0, 1024 );   
    int rpm = fan->getCurrentRPM();
    int pwr = fan->getCurrentPower();

    // Use the lower line to create a power bar that indicate power to stirplate
#if LOG_LEVEL==6
    Log.verbose(F("MAIN: POT = %d, Percentage %d, RPM=%d." CR), vin, pwr, rpm);
#endif

#ifdef ACTIVATE_BLYNK
    blynk.writeRemoteRPM( rpm );
    blynk.writeRemotePower( pwr );
#endif

    display->printProgressBar(0,1, pwr);

    // If there is no power we show the version number, else the RPM
    char s[10];

    if( fan->getCurrentPower() < 10 ) {    // Show the version number 
      sprintf( &s[0], "%5s", CFG_APPVER);
    }
    else {
      sprintf( &s[0], "%5d", fan->getCurrentRPM());
    }
    display->printText( 11, 0, &s[0] );

    lastMillis = millis();
  }
}

// EOF