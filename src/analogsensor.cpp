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
#include <espframework.hpp>
#include <helper.hpp>

AnalogSensor myAnalogSensor;

void AnalogSensor::setup() {
#if LOG_LEVEL == 6
  Log.verbose(F("ASEN: Setting up analog sensor." CR));
#endif
  pinMode(A0, INPUT);
#ifdef SIMULATE_SENSOR
  simualteMillsStart = millis();
#endif
}

int AnalogSensor::readSensor() {
#if LOG_LEVEL == 6
  Log.verbose(F("ASEN: Reading analog sensor." CR));
#endif

#ifdef SIMULATE_SENSOR
  value = simulateSensor();
  return value;
#endif

  //
  // This code will read the power value from the potentiometer and map that
  // towards a PWN signal for the fan + write the current level to the display
  //
  // Input will vary between 0-1024 and output should be limited to 0-255
  // according to documentation.
  //
  value = analogRead(A0);
  return value;
}

#ifdef SIMULATE_SENSOR

// Format, timestamp(s)-value
struct SimPoint {
  uint32_t time;
  int power;
};

const SimPoint simSequenceSensor[] = {
    {0, 0},    {15, 200}, {20, 300}, {25, 400}, {30, 500},
    {45, 600}, {50, 700}, {60, 800}, {70, 900}, {80, 1000},
};

int AnalogSensor::simulateSensor() {
  uint32_t tstamp =
      abs(static_cast<int32_t>(millis() - simualteMillsStart)) / 1000;
  int max = (sizeof(simSequenceSensor) / sizeof(SimPoint));
  int v = simSequenceSensor[max - 1].power;

  for (int i = 0; i < max; i++) {
    if (simSequenceSensor[i].time <= tstamp) v = simSequenceSensor[i].power;
  }

#if LOG_LEVEL == 6
  Log.verbose(F("ASEN: Value simulator %l, value %d." CR), tstamp, v);
#endif
  return v;
}

#endif  // SIMULATE_SENSOR

// EOF
