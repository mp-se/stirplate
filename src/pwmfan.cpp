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
#include "pwmfan.h"
#include "helper.h"

PwmFan myFan;

//
// Constructor
//
PwmFan::PwmFan() {
#if LOG_LEVEL==6
//    Log.verbose(F("Setting up pwm fan control." CR));
#endif
    pinMode(PwmFan::pwmCtrlPIN, OUTPUT);   
    analogWriteFreq( PwmFan::pwmFrequency );   
    analogWrite(PwmFan::pwmCtrlPIN, 0);
    rpmLastMillis = millis();
#ifdef SIMULATE_RPM
    simualteMillsStart = millis();
#endif
}

//
// Set the speed of the pwmsignal
//
int PwmFan::setPower( int value, int minRange, int maxRange ) {
#if LOG_LEVEL==6
//    Log.verbose(F("PFAN: Setting output value to %d." CR), value);
#endif
    powerPercentage = map(value, minRange, maxRange, 0, 100);   // Convert to percentage

    if( powerPercentage > 100 )     // Cap the value in case we get a higher analog read than expected.
        powerPercentage = 100;

    int power = map(powerPercentage, 0, 100, PwmFan::pwmMin, PwmFan::pwmMax);
    analogWrite(PwmFan::pwmCtrlPIN, power);
    return powerPercentage;
}

//
// Calculate the rotations per period
//
void PwmFan::loop() {
#ifdef SIMULATE_RPM
    rpm = simulateRPM();
    return;
#endif

    // How much time has passed since the last call?
    long unsigned timePeriod = millis() - rpmLastMillis;

#if LOG_LEVEL==6
    Log.verbose(F("PFAN: Period %d, Rotations = %d." CR), timePeriod, pwmRotationCounter);
#endif

    // My fan report 2 ticks per rotation. 
    rpm = ((double) pwmRotationCounter / (double) timePeriod) * 1000 * 60 / 2;
    pwmRotationCounter = 0;
    rpmLastMillis = millis();  
}

#if defined( SIMULATE_RPM )

// Format, powerpercentage-rpm
int simSequenceRPM[][2] = {
    {   0,    0 },
    {  10,    0 },
    {  20,  100 },
    {  30,  300 },
    {  40,  500 },
    {  50,  700 },
    {  60, 1100 },
    {  70, 1500 },
    {  80, 1800 },
    {  90, 2200 },
    { 100, 2900 },
};

//
// Used to simualate sensor value if no analog input is avaialble.
//
int PwmFan::simulateRPM() {
    int max = (sizeof(simSequenceRPM) / sizeof(int) / 2);

//  Log.verbose(F("PFAN: Value simulator %d." CR), powerPercentage);

    for(int i = 0; i < max; i++ ) {
        if( powerPercentage < simSequenceRPM[i][0] )
            return simSequenceRPM[i][1];
    }

    return simSequenceRPM[max-1][1];
}

#endif // SIMULATE_RPM

// EOF 