/* Library to interface with Maxim ICM7218C LED driver chip.
   
   Original library by Andreas Taylor <Andy4495@outlook.com>

   Note that there are several chips in the 72xx family, including
   chips from Maxim.
   While these other chips provide similar capabilities, this library
   was specifically designed for the Maxim ICM7218C variant.
   This library will function properly with other A/B chips and may
   not function out of the box for the D varient..
   https://facebook.com/werelwolf

   Interface requires up to 10 digital out pins for full support of
   all functionality. However, it is possible to use as few as 7 output
   pins if no decimal point is needed and are only driving 2 segment(s).
   Library has been coded to require at minimum two 7-segment displays.
*/

/* Version History
   1.0.0    02/12/2018  A.T.   Original
   1.1.0    02/27/2018  A.T.   Added support for ascii to segment mapping
   1.2.0    08/07/2018  A.T.   Changed names of shutdown() and wakeup() to
                               displayShutdown() and displayWakeup()
                               Renamed segment_map to ICM7218_segment_map
   1.2.3    03/23/2022  W.W.   Changing library for Maxim ICM7218C chip.
*/

/* Constructor:
      D2 - D5   : digital output data pins for display data. D2 is
                    least significant bit.
      D6        : digital output data pin for Active Low decimal point
                   Use ICM7218C::NO_PIN if pin is not connected to Arduino
      D7        : digital output data pin for segment digit addressing.
                   D7 is the least significant digit.
      D8        : digital output data pin for segment digit addressing.
                   Use ICM7218C::NO_PIN if pin is not connected to Arduino
      D9        : digital output data pin for segment digit addressing.
                   Use ICM7218C::NO_PIN if pin is not connected to Arduino
      mode_pin  : digital output pin for MODE signal
      RF_pin    : digital output pin for /WRITE signal
*/

#include "Arduino.h"
#include "ICM7218C.h"

/*
   What Arduino pins are connected to ISM7218C in order:
     ID0, ID1, ID2, ID3, !ID7, DA0, DA1, DA2, HEX/CODE/!SD, !WRITE
*/

ICM7218C::ICM7218C(byte D2_pin, byte D3_pin, byte D4_pin, byte D5_pin,
                      byte D6_pin, byte D7_pin, byte D8_pin, byte D9_pin,
                      byte mode_pin, byte write_pin)
{
  d2_out =             D2_pin;         // ID0 - 7218C pin 12 -lsb
  d3_out =             D3_pin;         // ID1 - 7218C pin 11
  d4_out =             D4_pin;         // ID2 - 7218C pin 13
  d5_out =             D5_pin;         // ID3 - 7218C pin 14 -msb
  d6_out =             D6_pin;         // ID7 - 7218C pin 7 -!decimal point
  d7_out =             D7_pin;         // DA0 - 7218C pin 5 -lsb
  d8_out =             D8_pin;         // DA1 - 7218C pin 6
  d9_out =             D9_pin;         // DA2 - 7218C pin 10 -msb
  mode_out =           mode_pin;       // HEXA/CODE B/!SHUTDOWN pin 9
  write_out =          write_pin;      // !WRITE pin 8 -Active low

  digitalWrite(write_out, HIGH);  // Make sure /WRITE signal is inactive
  pinMode(write_out, OUTPUT);

  // Data pin levels don't need to be set in constructor, since they are only
  // latched by ICM7218C when the /WRITE signal is low.
  pinMode(d2_out, OUTPUT);
  pinMode(d3_out, OUTPUT);
  pinMode(d4_out, OUTPUT);
  pinMode(d5_out, OUTPUT);
  if (d6_out != NO_PIN) pinMode(d6_out, OUTPUT);   // !decimal point
  pinMode(d7_out, OUTPUT);
  if (d8_out != NO_PIN) pinMode(d8_out, OUTPUT);
  if (d9_out != NO_PIN) pinMode(d9_out, OUTPUT);

  digitalWrite(mode_out, LOW);  // WW Make sure mode_out starts floating.
  pinMode(mode_out, INPUT);     // WW Changed OUTPUT to INPUT to insure floating pin. A
                                // digitalWrite() of HIGH will enable the internal
                                // pullup resistor which may register as Logical "1" for
                                // Hex mode selectaion. A floating pin is registered as
                                // Code B, Logical "0" as Shutdown.  As INPUT can only
                                // be floating or pullup, Shutdown is not possible.

  mode = CODEB;            // Default mode is CODEB decode until changed with setMode()
  power_state = WAKEUP;    // Default power state is active until changed with shutdown()

}

void ICM7218C::setMode(CHAR_MODE m) {
  switch (m) {
    case CODEB:
      digitalWrite(mode_out, LOW);
      mode = m;
      //Serial.println("setMode Mode is now CODEB.");
      break;
    case HEXA:
      digitalWrite(mode_out, HIGH);
      mode = m;
      //Serial.println("setMode Mode is now HEXA.");
      break;
    default:
      //Serial.println("setMode returned default.");
      break;
  }

  //Serial.print("setMode mode set: ");  Serial.println(mode);
}



void ICM7218C::print(const char* s) {
  byte outbuf[8]; // one encoded character (with/o dp) for every possible display
  int outbuf_len;
  int index = 0;
  int i = 0;
  byte DP;


    switch (mode) {
    case CODEB:
      //Serial.print("In CODEB case, mode is: ");  Serial.println(mode);
      memset(outbuf, 0x0F, 8); // Initialize to default characters (<space>)
      while (index < 8) {
        DP = 0;
        if (s[i+1] == '.') DP = 0b00010000;
        switch (s[i]) {
            case '0':  case '1': case '2': case '3': case '4':
            case '5':  case '6': case '7': case '8': case '9':
              outbuf[index++] = (s[i] - '0') | DP;
              break;
            case '-':
              outbuf[index++] = 10 | DP;
              break;
            case 'E':  case 'e':
              outbuf[index++] = 11 | DP;
              break;
            case 'H': case 'h':
              outbuf[index++] = 12 | DP;
              break;
            case 'L': case 'l':
              outbuf[index++] = 13 | DP;
              break;
            case 'P': case 'p':
              outbuf[index++] = 14 | DP;
              break;
            case ' ':
              outbuf[index++] = 15 | DP;
              break;
            case '.':
              // Just skip decimal place and do not encode a new byte.
              break;
            case '\0':     // End of string
              index = 8;   // This will end the while loop
              break;
            default:       // Invalid character printed as a blank
              //index++;
              outbuf[index++] = 15 | DP;
              break;
        }
        
        i++;
      }
      break;

    case HEXA:
      //Serial.print("In HEXA case, mode is: ");  Serial.println(mode);
      memset(outbuf, 0x00, 8); // Initialize to default characters (0)
      while (index < 8) {
        DP = 0;
        if (s[i+1] == '.') DP = 0b00010000;
        switch (s[i]) {
            case '0': case '1': case '2': case '3':  case '4':
            case '5': case '6': case '7': case '8':  case '9':
              outbuf[index++] = (s[i] - '0') | DP;
              break;
            case 'A':  case 'B': case 'C': case 'D': case 'E': case 'F':
              outbuf[index++] = (char(s[i] - 'A') + 10) | DP;
              break;
            case 'a':  case 'b': case 'c': case 'd': case 'e': case 'f':
              outbuf[index++] = (char(s[i] - 'a') + 10) | DP;
              break;
            case '.':
              // Just skip decimal place and do not encode a new byte.
              break;
            case '\0':      // End of string
              index = 8;    // This will end the while loop
              break;
            default:        // Invalid character, use default character (0)
              outbuf[index++] = 0 | DP;
              break;
        }
        i++;
      }
      break;

    default:
      //Serial.print("In default case, mode is: ");  Serial.println(mode);
      memset(outbuf, 0x17, 8); // Initialize to error display "7."
      break;

  }
/*
  // Send whole outbuf[] to console.
  Serial.print("outbuf[] is: ");
  for (i = 0; i < 8; i++) {
    for (int b = 7; b >= 0; b--) {
      Serial.print(bitRead(outbuf[i], b));
    } Serial.print(" ");
  }
  Serial.println(" ");
*/


  // Truncate the data
  outbuf_len = 8;
  if (d9_out == NO_PIN) outbuf_len = 4;
  if (d8_out == NO_PIN) outbuf_len = 2;
  
  // Send the data
  for (i = 0; i < outbuf_len; i++) {
    segmentWrite(outbuf[i], i);
  }
}

void ICM7218C::displayShutdown() {
  power_state = SHUTDOWN;
  pinMode(mode_out, OUTPUT);
  digitalWrite(mode_out, LOW);
}

void ICM7218C::displayWakeup() {
  power_state = WAKEUP;
  pinMode(mode_out, INPUT);
  digitalWrite(mode_out, mode);   // Re-enables the state it was in via pullup.
}



void ICM7218C::segmentWrite(byte b, int seg) {   // private func
  // CODEB: 0b00011000 = all segments ON ; 0b***01111 = all segments OFF ;
  // HEXA:  0b00011000 = all segments ON ; no value for all segments OFF ;

  digitalWrite(d2_out, ( (b & 1 ) != 0) );
  digitalWrite(d3_out, ( (b & (1 << 1) ) != 0) );
  digitalWrite(d4_out, ( (b & (1 << 2) ) != 0) );
  digitalWrite(d5_out, ( (b & (1 << 3) ) != 0) );
  if (d6_out != NO_PIN) digitalWrite(d6_out, ( (b & (1 << 4) ) == 0) );
  digitalWrite(d7_out, ( (seg & 1 ) != 0) );
  if (d8_out != NO_PIN) digitalWrite(d8_out, ( (seg & (1 << 1) ) != 0) );
  if (d9_out != NO_PIN) digitalWrite(d9_out, ( (seg & (1 << 2) ) != 0) );

  // Latch in the data
  digitalWrite(write_out, LOW);
  digitalWrite(write_out, HIGH);
  
}


// End of ICM7218C.cpp