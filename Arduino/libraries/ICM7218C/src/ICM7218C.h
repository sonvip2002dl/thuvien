/* Library to interface with Maxim ICM7218C LED driver chip.
   
   Original library by Andreas Taylor <Andy4495@outlook.com>

   Note that there are several chips in the 72xx family, including
   chips from Maxim.
   While these other chips provide similar capabilities, this library
   was specifically designed for the Maxim ICM7218C variant.
   This library may require modifications to function properly with other
   chips.
   https://facebook.com/werelwolf

   Interface requires up to 12 digital out pins for full support of
   all functionality. However, it is possible to use as few as 6 output
   pins if only a single decoded character set is needed.

   Aiming to only require 10 pins or less.
*/
/* Version History
   1.0.0    02/12/2018  A.T.   Original
   1.1.0    02/27/2018  A.T.   Added ascii to segment mapping table
   1.2.0    08/07/2018  A.T.   Changed names of shutdown() and wakeup() to
                               displayShutdown() and displayWakeup()
                               Moved segment_map out of class definition and
                               renamed to ICM7218_segment_map
   1.2.3    03/23/2022  W.W.   Changing library for Maxim ICM7218C chip.
*/
#ifndef ICM7218C_LIBRARY
#define ICM7218C_LIBRARY

#include <Arduino.h>

class ICM7218C {

public:
  enum CHAR_MODE {HEXA=1, CODEB=0};
  enum {NO_PIN=255};
  ICM7218C(byte D2, byte D3, byte D4, byte D5, byte D6, byte D7, byte D8,
          byte D9, byte mode_pin, byte write_pin);
  void setMode(CHAR_MODE);
  void print(const char*);
  void displayShutdown();
  void displayWakeup();

private:
  enum POWER_MODE {WAKEUP = 1, SHUTDOWN = 0};
  byte d2_out, d3_out, d4_out, d5_out, d6_out, d7_out, d8_out, d9_out;
  byte mode_out;
  byte write_out;
  int mode;
  int power_state;
  void segmentWrite(byte b, int seg);
};

#endif


// End of ICM7218C.h