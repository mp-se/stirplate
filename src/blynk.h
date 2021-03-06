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
#ifndef _BLYNK_H
#define _BLYNK_H

#if defined( ACTIVATE_BLYNK ) && defined( ACTIVATE_WIFI )

// Includes
#include <Arduino.h>

// Classes
class BlynkPins {
    private:
        int toggle  = 0;
        int power   = 0;
        bool active = false;

    public:
        BlynkPins() {};
        void connect(const char* blynkToken, const char* ip, int port);
        void loop();

        void setRemoteToggle(int t) { toggle = t; };
        void setRemotePower(int p) { power = p; }

        int readRemoteToggle() { return toggle; };
        int readRemotePower() { return power; }
        void writeRemoteRPM(int v);
        void writeRemotePower(int v);
        void writeRemoteVer(const char *ver);
        void writeRemoteTempC(float f);
        void writeRemoteTempF(float f);

        bool isActive() { return active; };
};

extern BlynkPins myBlynk;

#endif // ACTIVATE_BLYNK && ACTIVATE_WIFI

#endif // _BLYNK_H

// EOF