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
#include <analogsensor.hpp>
#include <display.hpp>
#include <espframework.hpp>
#include <helper.hpp>
#include <main.hpp>
#include <ota.hpp>
#include <pwmfan.hpp>
#include <stirconfig.hpp>
#include <stirwebhandler.hpp>
#include <tempsensor.hpp>
#include <wificonnection.hpp>

SerialDebug mySerial(115200L);
StirConfig myConfig(CFG_MDNSNAME, CFG_FILENAME);
WifiConnection myWifi(&myConfig, CFG_APPNAME, "password", CFG_MDNSNAME);
OtaUpdate myOta(&myConfig, CFG_APPVER);
StirWebHandler myWebHandler(&myConfig);

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
#define LOOP_INTERVAL 100  // ms, time to wait between running the loop code
static const int tachPIN = 12;  // Measure speed on FAN. D6 PIN on ESP-12F

void IRAM_ATTR handleTachiometerInterrupt() { myFan.tachCallback(); }

void setup() {
  // Initialize pin outputs
  Log.notice(F("Main: Started setup for %s." CR),
             String(ESP.getChipId(), HEX).c_str());

  Log.notice(F("Main: Loading configuration." CR));
  myConfig.checkFileSystem();
  myConfig.loadFile();
  myWifi.init();

  Log.notice(F("Main: Setting up devices." CR));

  ESP.wdtDisable();
  ESP.wdtEnable(LOOP_INTERVAL * 2);

  Log.notice(F("Main: Looking for display." CR));
  myDisplay.init();

  char buffer[20];
  snprintf(&buffer[0], sizeof(buffer), "%s", CFG_APPNAME);
  myDisplay.printText(0, 0, &buffer[0]);

  if (!myWifi.hasConfig() || myWifi.isDoubleResetDetected()) {
    Log.notice(
        F("Main: Missing wifi config or double reset detected, entering wifi "
          "setup." CR));
    myDisplay.printText(0, 1, "Wifi portal...  ");
    myWifi.startPortal();
  }

  myDisplay.printText(0, 1, "Connect wifi    ");
  myWifi.connect();

  if (!myWifi.isConnected() || myOta.checkFirmwareVersion()) {
    Log.notice(F("Main: New firmware available via OTA, doing update." CR));
    delay(500);
    myDisplay.printText(0, 1, "Updating        ");
    myOta.updateFirmware();
  }

  if (myWifi.isConnected()) {
    myWebHandler.setupWebServer();
    Log.notice(F("Main: Connected to wifi ip=%s." CR),
               myWifi.getIPAddress().c_str());
  }

  // Setup interrput callback for tachiometer (fan rotation)
  pinMode(tachPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(tachPIN), handleTachiometerInterrupt,
                  FALLING);

  Log.notice(F("Main: Setting up sensors." CR));
  myAnalogSensor.setup();
  myTempSensor.setup();

#if LOG_LEVEL == 6
  Log.verbose(F("MAIN: Wait 5s." CR));
  delay(5000);
#endif
  Log.notice(F("Main: Setup is completed." CR));
}

// Variables used in the main loop
int loopCounter = 0;      // used in the loop
int loopTempCounter = 0;  // used to swap between temp and rpm
uint32_t loopLastMillis = 0;
int loopLastVin = 0;  // Last value of analog pot (used to force display update)

void loop() {
  if (myWifi.isConnected()) {
    myWifi.loop();
    myWebHandler.loop();
  }

  // We dont run this in a tight loop, every 500 ms is fast
  if (abs(int32_t(millis() - loopLastMillis)) > LOOP_INTERVAL) {
    // Reset the counter
    loopLastMillis = millis();

    // We vary what parts of the code is run every main loop. Some parts like
    // display updates/temp reading can be done more seldom.
    loopCounter++;

    // This code is run every loop.
    // -----------------------------------------------------------
    // setPower will map the value to the range supported by the PWM output
    int vin = myAnalogSensor.readSensor();
    int rpm = myFan.getCurrentRPM();
    int pwr = myFan.getCurrentPower();

    // Update the power setting based on current pot reading
    myFan.setPower(vin, 0,
                   POT_MAX_READING);  // (value, min,  max) reading of value

    // Use the lower line to create a power bar that indicate power to stirplate
#if LOG_LEVEL == 6
    Log.verbose(F("Loop: POT = %d, Percentage %d, RPM=%d." CR), vin, pwr, rpm);
#endif

    bool tempAttached = myTempSensor.isSensorAttached();
    float tempC = myTempSensor.getValueCelcius();
    float tempF = myTempSensor.getValueFarenheight();

    // This code is run every 1 seconds or when we have an updated power setting
    // -------------------------------------------------------------------------
    if (!(loopCounter % 10) || vin != loopLastVin) {
#if LOG_LEVEL == 6
      Log.verbose(F("Loop: Running 1 second loop." CR));
#endif
      loopLastVin = vin;
    }

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
    // 1|Stir plate  0rpm| - RPM (Variera var 5:e sekund med TEMP)
    // 2|-progress-  000%| - Progress + % power

    //  |123456789.123456| - Power on
    // 1|Stir plate   20C| - Temperator (Variera var 5:e sekund med RPM) -
    // Välja temperatur (C/F) 2|-progress-  000%| - Progress + % power

    // If there is no power we show the version number, else the RPM

    char s[10];

    // If the stirplate is not running we show the version number
    if (myFan.getCurrentPower() < 10) {
      snprintf(&s[0], sizeof(s), "%5s", CFG_APPVER);

      if (myWifi.isConnected())
        myDisplay.printText(0, 1, "WIFI            ");
      else
        myDisplay.printText(0, 1, "NO WIFI         ");
    } else {
      // Create progress bar
      myDisplay.printProgressBar(0, 1, pwr);

      // Show RPM
      snprintf(&s[0], sizeof(s), "%5d", myFan.getCurrentRPM());

      // Show Temp
      if (loopTempCounter++ > 10) loopTempCounter = 0;

      // Every 5 seconds we swap between RPM and TEMP if there is a temp
      // sensor attached
      if ((loopTempCounter > 5) && tempAttached) {
        if (myConfig.isTempC())
          snprintf(&s[0], sizeof(s), "%3d C", static_cast<int>(tempC));
        else
          snprintf(&s[0], sizeof(s), "%3d F", static_cast<int>(tempF));
      }
    }

    myDisplay.printText(11, 0, &s[0]);
  }

  // This code is run every 5 seconds.
  // -----------------------------------------------------------
  if (!(loopCounter % 50)) {
#if LOG_LEVEL == 6
    Log.verbose(F("MAIN: Running 2,5 second loop." CR));
#endif
    // Used to calculate the RPM value.
    myFan.loop();
  }

  // This code is run every 10 seconds.
  // -----------------------------------------------------------
  if (!(loopCounter % 100)) {
    // Since the tempsensor can be remove/added we check if there is a change
    myTempSensor.setup();
  }
}

// EOF
