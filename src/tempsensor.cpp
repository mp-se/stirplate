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
#include "tempsensor.h"

//
// THIS CODE HAS NOT YET BEEN TESTED IN THE BUILD. FUTURE FEATURE
//

/*
OneWire oneWire(D6);
DallasTemperature sensors(&oneWire);
*/

//
//
//
TempSensor::TempSensor() {
/*
#if LOG_LEVEL==6
  Log.verbose(F("TSEN: Looking for temp sensors." CR));
#endif

  sensors.begin();
  noSensors = sensors.getDeviceCount();
  Log.notice(F("TSEN: Found %d sensors." CR), noSensors);
  sensors.setResolution(TEMPERATURE_PRECISION);
#if LOG_LEVEL==6
  Log.verbose(F("TSEN: Set resolution to %d." CR), TEMPERATURE_PRECISION);
#endif
  run();

  for( int i=0; i<noSensors; i++) {
    DeviceAddress da;

    if(!sensors.getAddress(da, i)) {
      Log.error(F("Temp: Unable to find address for device %d." CR), i);
    } else {
      Log.notice(F("Temp: Device [%d] adress %x %x %x %x %x %x %x %x." CR), i, 
        da[0],da[1],da[2],da[3],da[4],da[5],da[6],da[7] );

#if LOG_LEVEL==6
      Log.verbose(F("Temp: Temperature for device [%d] %F C." CR), i, values[i]);
#endif
    }
  }
*/
}

//
// Reading values from temp sensors
//
void TempSensor::run() {
/*
  sensors.requestTemperatures();

  for( int i=0; i<noSensors; i++) {

    float tempC = sensors.getTempCByIndex(i);
    if(tempC == DEVICE_DISCONNECTED_C) {
      Log.error(F("TSEN: Could not read temperature data from device [%d]." CR), i);
    } else {
      values[i] = tempC;
#if LOG_LEVEL==6
      Log.verbose(F("TSEN: Temperature for device [%d] %F C." CR), i, getValue(i));
#endif
    } 
  }
*/
}

//
// Retrieving value from sensor
//
float TempSensor::getValue(int index) {
/*
  float f = 0;

  if( noSensors >= index )
    f = values[index];

#if LOG_LEVEL==6
  Log.verbose(F("TSEN: Reciving temp value for sensor %d = %F C." CR), index, f);
#endif
  return f;
*/
  return 0;
}

// EOF 