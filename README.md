# PWM controlled stir plate for Beer Brewing

I decided to build my own stir plate and I wanted to use a PWM controlled fan for optimal performance. This also enabled me to measure RPM of the fan. I'm using this build for my yeast starters. Tests shows that this setup can handle a starter for up to 3 liters (I dont have a bigger glass vessel to test with).

Since I had a few ESP8266 controller this was the natual choise to use and since there is built in wifi, one needs to use that. This version can communicate with Blynk server in order to display the RPM and also control the FAN from the app. 

![Yeast fermentation](img/stirplate.jpg)

Here is a short video that shows the minimum and maximum speed with a 3 liter starter. The slow speed is around 150 RPM and max speed is 1400 RPM.

[Video of running stir plate](img/speed.mov)

## Versions

* 0.6.0 Added support for http endpoints + web api + testsuite + blynk cloud support
* 0.5.0 Added temp sensor support + pcb layout (pcb has not yet been fully tested)
* 0.4.0 Added wifi manager/web server to enable config changes via webbrowser/wifi setup.
* 0.3.0 Some minor refactoring + OTA update from local web server.
* 0.2.0 First version that is published on github

## How it works

I use a potentimeter (5k) to control the speed, this is read via the Analog input (A0) on the ESP (note that this is limited to max 3.3V) so if it's powered by 5V a voltage divider must be used (R1 is used to limit the input to 3.3V). 

D5 is used to generate the PWM signal to the FAN and D6 is used to monitor the RPM via interrupt. So the basic design is quite simple. D1 and D2 are used for communicating with the LCD display via I2C.

There are 3 defined targets in the platformIO configuration

* Release with WIFI (firmware.bin)
* Release without WIFI (firmware-nowifi.bin)
* Debug version uses simualted values to simplify development.

## Installation

You can use VisualStudio Code + Platform IO to handle the device flashing. 

An option could be to use this tool; https://github.com/marcelstoer/nodemcu-pyflasher

## Setup

The wifi version will create an WIFI access point at startup called StirPlate (password=password). In the portal you can choose some of the settings. 

Double tap on the reset button will force the device into wifi setup mode. See below for the description of the configuration paramaters.

Once the device is on the wifi network it will have a running webserver that can show the active configuration and also force the device into configuration model. The name of the device will be __stirplateXXXXX.local__ (or just use the dynamic IP). Chip ID will be 6 characters and uniqe for that device (eg 7a84DC).

* __/__ will show the name, version and chip ID
* __/config__ will show the current configuration in json format
* __/reset?id=X__ will reboot the device.
* __/clearwifi?id=X__ will force the device into wifi configuration mode by erasing the wifi settings.
* __/api/config/get?param=Y__ will receive a configuration parameter.
* __/api/config/set?id=X&param=Y&value=Z__ will set a configuration parameter. 

The ID parameter is used to validate that the commands are for the correct device (ID = ChipID). This can be found on the main page or via the /config page.

Valid configuration parameters:

* __id__ Chip ID (Read Only)
* __mdns__ mDNS name of the device (Changing this via API requires a reboot to take affect) *
* __otaurl__ url to directory where new firmware versions are located *
* __blynkserver__ adress of remote server (if this is left empty, blynk cloud will be used) *
* __blynkserverport__ port for remote blynk server (only used if blynkserver is defined, must be a valid port number) *
* __blynktoken__ blynk token (if defined blynk support is active) 
* __httppush__ url to brewfather endpoint (if defined http push will be active)
* __pushinterval__ seconds between push (only applies to http push)
* __tempformat__ temperature format (Valid: C or F)

\* These parameters require a reboot for the change to take affect. 

## Build Configuration

I prefer to use Visual Studio Code with the extension PlatformIO which makes it quite easy to make a build.

The following defintions can be used to enable/disable parts of the code

* ACTIVATE_BLYNK    Include blynk code in build (requires wifi)
* ACTIVATE_OTA      Include ota code in build (requires wifi)
* ACTIVATE_PUSH     Include support for push targets (requires wifi) 
* ACTIVATE_WIFI     Include wifi access in build 
* ACTIVATE_TEMP     Include temperature sensor access in build 

Development related settings (so I dont have to have a fully working setup)

* LOG_LEVEL=6       Configure Arduino Log (6=Debug, 5=Trace, 4=Notice, 3=Warning, 2=Error, 1=Fatal, 0=Silent)
* SIMULATE_SENSOR   Used to simulate pot readings
* SIMULATE_TEMP     Used to simulate temp readings
* SIMULATE_RPM      Used to simulate pwm readings
* DISPLAY_SELFTEST  Runs some tests on the display at startup

## OTA function

From version 0.3.0 I have added the possibility to do updates via OTA from a local web server over port 80. 

For this to work, place the following files (version.json + firmware.bin) at the location that you pointed out in the mysecrets.h file. If the version number in the json file is newer than in the code the update will procced.

Example; OTA URL (don't forget trailing dash), the name of the file should be firmware.bin
```
http://192.168.4.1/firmware/stirplate/
```

Contents version.json
```
{ "project":"stirplate", "version":"0.3.0" }
```

## Blynk Sensors

Currently the code uses the following virtual sensors to interact with blynk

* V0 - Output - Fan RPM 
* V1 - Output - Potentiometer Fan Power in %
* V2 - Input  - Activate remote control (if = 1, it will use V3 to control the speed)
* V3 - Input  - Remote Fan Power in %
* V4 - Output - Version string
* V5 - Output - Temp sensor in C (if no tempsensor attached it will return 0)
* V6 - Output - Temp sensor in F (if no tempsensor attached it will return 32)

![Screenshot from Blynk](img/blynk.png)

## HTTP Push Target

When submitting data to a http endpoint the data is submitted in json format;

```
{
    "name" : "mydevice",
    "temp" : 28.5,
    "temp_unit" : "C',
    "rpm" : 1200,
    "rssi" : -58,
}
```

## Materials

In order to build this project you will need the following;

Schema and PCB is avaialble under the cad folder, these are designed in Eagle (Free version) and can be downloaded from Autodesk. Note! That these are not yet tested and verified. 

* U1 - Microcontroller ESP8266 (WEMOS D1 Mini)
* IC1- DS28B20 (Temperature probe)
* IC2- Power regulator 12V -> 5V (I used LD1085V50)
* R1 - Resistor 3.3k 
* R2 - Potentiometer 5k
* R3 - Resistor 3.3k
* R4 - Resistor 4.7k
* C1 - 10uF
* C2 - 10uF
* 2 x 4PIN connectors for display and fan.
* I2C compatible display - I used a 2x16 LCD
* Board for mounting components
* FAN with PWM (4-pin) - I used Noctua NF-B9 rexux 1600 
* Box large enough to hold the FAN and electronics
* Power supply 12VAC - 1A
* 2 neodym magnets (glued to fan)

A schematics that I used can be found in the cad directory. [Electronic schema](cad/stirplate.pdf)

![First build](img/build.jpg)

On my build I cut of the wings to the FAN, this increased the maximum RPM from 1800 -> 2700. I glued the fan to the box lid,, just make sure there is enough distance for the fan to spin. 

![Fan without wings](img/fan.jpg)

Feel free to use the code and modify your own build. 

Happy building. /Magnus 