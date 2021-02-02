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
#include "helper.h"

#include <onewire.h>
#include <DallasTemperature.h>
#include <Wire.h>

#if defined( ACTIVATE_TEMP )

OneWire oneWire(D4);
DallasTemperature sensors(&oneWire);
TempSensor stirTempSensor;

#define TEMPERATURE_PRECISION 9

//
// Setup temp sensors
//
void TempSensor::setup() {
#if LOG_LEVEL==6
  Log.verbose(F("TSEN: Looking for temp sensors." CR));
#endif
  sensors.begin();
  Log.notice(F("TSEN: Found %d sensors." CR), sensors.getDeviceCount());
  sensors.setResolution(TEMPERATURE_PRECISION);
}

//
// Retrieving value from sensor
//
float TempSensor::getValueCelcius() {
  float f = 0;

  sensors.requestTemperatures();

  if( sensors.getDeviceCount() >= 1) {
    f = sensors.getTempCByIndex(0);

#if LOG_LEVEL==6
    Log.verbose(F("TSEN: Reciving temp value for sensor %F C." CR), f);
#endif
  }

  return f;
}

#endif //  ACTIVATE_TEMP 

// EOF 