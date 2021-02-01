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
#ifndef _DISPLAY_H
#define _DISPLAY_H

// includes
#include <LiquidCrystal_I2C.h>

// classes
class Display {
    private:
        LiquidCrystal_I2C *lcd;
        bool lcd_found = false;

        void setup();
        void createCustomChars();
        
#ifdef DISPLAY_SELFTEST 
        void selfTest();
#endif

    public:
        const static int DISP_COLUMS = 16;
        const static int DISP_ROWS   = 2;

        Display();
        void clear();

        void printText( int x, int y, const char *s );
        void printProgressBar( int x, int y, int percentage );
};

#endif // _DISPLAY_H

// EOF