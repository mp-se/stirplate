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
#include "display.h"
#include "helper.h"
#include <Wire.h>                   

Display myDisplay;

//
// Initialize display
//
void Display::init() {
//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Initiating LCD display." CR));
//#endif
  lcd = new LiquidCrystal_I2C(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
  setup();
  createCustomChars();
#ifdef DISPLAY_SELFTEST 
  selfTest();
#endif
  clear();
}

//
// Testing functionality of the displayclass
//
#ifdef DISPLAY_SELFTEST 
void Display::selfTest() {
  printText( 0, 0, "123456789.123456" );
  printText( 0, 1, "ABCDEFGHIJKLMNOP" );
  delay(2000);

  for( int i = 0; i < 100; i += 2 ) {
    printProgressBar( 0, 1, i );
    delay(500);
  }

  printProgressBar( 0, 1, 100 );
  delay(5000);
}
#endif // DISPLAY_SELFTEST


//
// Setup the display, choose the size in config.h
//
void Display::setup() {
  Log.notice(F("DISP: Setting up LCD display." CR));
  int retry = 0;

  while (lcd->begin(DISP_COLUMS, DISP_ROWS) != 1) {

    if( retry++ > 5 ) {
      Log.warning(F("DISP: Display not found, tried several times to initiate, continue without display." CR));
      return;
    }

    Log.error(F("DISP: PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal." CR));
    delay( 50 );
  }

  lcd_found = true;
}

// Custom characters for the progress bar 
byte START_DIV_0_OF_1 [ 8 ] = {
  B01111, 
  B11000,
  B10000,
  B10000,
  B10000,
  B10000,
  B11000,
  B01111
}; // Char start 0/1

byte START_DIV_1_OF_1 [ 8 ] = {
  B01111, 
  B11000,
  B10011,
  B10111,
  B10111,
  B10011,
  B11000,
  B01111
}; // Char start 1/1

byte DIV_0_OF_2 [ 8 ] = {
  B11111, 
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111
}; // Char middle 0/2

byte DIV_1_OF_2 [ 8 ] = {
  B11111, 
  B00000,
  B11000,
  B11000,
  B11000,
  B11000,
  B00000,
  B11111
}; // Middle tank 1/2

byte DIV_2_OF_2 [ 8 ] = {
  B11111, 
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B00000,
  B11111
}; // Middle tank 2/2

byte END_DIV_0_OF_1 [ 8 ] = {
  B11110, 
  B00011,
  B00001,
  B00001,
  B00001,
  B00001,
  B00011,
  B11110
}; // Char end 0/1

byte END_DIV_1_OF_1 [ 8 ] = {
  B11110, 
  B00011,
  B11001,
  B11101,
  B11101,
  B11001,
  B00011,
  B11110
}; // Char thin 1/1

//
// Create the custom chars used in the progress bar
// Use custom characters 0 to 6 (7 remains available). 
//
// Code for progress bar is inspired from; 
// https://www.carnetdumaker.net/articles/faire-une-barre-de-progression-avec-arduino-et-liquidcrystal/
//
void Display::createCustomChars() { 
//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Setting up custom characters." CR));
//#endif

  if( !lcd_found )
    return;

  lcd->createChar ( 0 , START_DIV_0_OF_1);
  lcd->createChar ( 1 , START_DIV_1_OF_1);
  lcd->createChar ( 2 , DIV_0_OF_2);
  lcd->createChar ( 3 , DIV_1_OF_2);
  lcd->createChar ( 4 , DIV_2_OF_2);
  lcd->createChar ( 5 , END_DIV_0_OF_1);
  lcd->createChar ( 6 , END_DIV_1_OF_1);
}

//
// clear the display
//
void Display::clear() {
//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Clear display." CR));
//#endif

  if( !lcd_found )
    return;

  lcd->clear();
}

//
// Print string
//
void Display::printText(int x, int y, const char *s ) {
//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Printing '%s' at %d,%d." CR), s, x, y);
//#endif

  if( !lcd_found )
    return;

  lcd->setCursor(x,y);
  lcd->print( s );    
}

// 
// Function drawing the progress bar. 
// 
// The progress bar is fixed to 10 chars (to make the rendering simpler)
// 
void Display::printProgressBar(int x, int y, int percentage) {
//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Creating progress %d at %d,%d." CR), percentage, x, y);
//#endif

  if( !lcd_found )
    return;

  // Move the cursor to the start of the progress bar 
  lcd->setCursor ( x , y );

  // This is the desired printing on the line, 10 chars progress bar + 4 chars for the value.
  // 
  // 123456789.123456
  // ----------  100%
  #define PROGRESS_BAR_WIDTH 10

  // Each character displays 2 vertical bars, but the first and last character displays only one.
  // Map range (0 ~ 100) to range (0 ~ LCD_NB_COLUMNS * 2 - 2) 
  int col = map(percentage, 0 , 100 , 0 , PROGRESS_BAR_WIDTH * 2  -  2 );

  // Print the progress bar 
  for (int i =  0 ; i < PROGRESS_BAR_WIDTH; ++ i) { 
    switch( i ) {
      case 0:
//#if LOG_LEVEL==6
//        Log.verbose(F("DISP: First poistion %d." CR), col);
//#endif
        // Char 0 = empty start, Char 1 = full start 
        lcd->write( col==0 ? 0 : 1 );
        col -= 1; // First item only have one halv bar
      break;

      case PROGRESS_BAR_WIDTH-1:
//#if LOG_LEVEL==6
//        Log.verbose(F("DISP: Last position %d." CR), col);
//#endif
        // Char 5 = full end, Char 6 = empty end 
        lcd->write( col>0 ? 6 : 5 );
      break;

      default:
//#if LOG_LEVEL==6
//        Log.verbose(F("DISP: Creating progress col=%d, loop=%d." CR), col, i);
//#endif
        if( col <= 0 ) {
          // Char 2 = empty middle 
          lcd->write ( 2 );
        } else {
          // Char 3 = half middle, Char 4 = full middle 
          lcd->write( col>=2 ? 4 : 3 );
          col -= 2; // One char equals to 1-2 indicators. 
        }
      break;
    }
  }

  // Write the percentage at the end of the line
  char s[6];
  sprintf( &s[0], "  %3d%%", percentage);

//#if LOG_LEVEL==6
//  Log.verbose(F("DISP: Printing percentage '%s'." CR), s);
//#endif
  lcd->print( s );
}

// EOF 