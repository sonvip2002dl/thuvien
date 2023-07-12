
//
// 1.0.0    02/27/2018  A.T.   Original
// 1.1.0    03/01/2018  A.T.   Add ASCII to segment mapping in DIRECT mode
// 1.2.0    08/07/2018  A.T.   Raname display() and wakeup() methods
// 1.2.3    03/23/2022  W.W.   Changing library for Maxim ICM7218C chip.


#include <ICM7218C.h>

// Shortens mode flags.
#define HEXA ICM7218C::HEXA
#define CODEB ICM7218C::CODEB
#define NO_PIN ICM7218C::NO_PIN

// Configure the 10 OUTPUT pins used to interface with ICM7218C: D2-D9, mode, write
//  Use NO_PIN if pin is not connected to Arduino //NOT YET IMPLIMENTED #############################
ICM7218C myLED(2, 3, 4, 5, 6, 7, 8, 9, 18, 19);


void setup() {

// Prints by default in CODE B mode.

  myLED.print("1.23PL");


/*
// Can change mode between CODEB and HEXA
  setMode(HEXA);

  myLED.print("1A2B3C4D");
 */


/*
// Has display Shutdown and Wakeup functions.
  myLED.print("0h2.3P.F65");

  delay(2500);

  myLED.displayShutdown();

  delay(4000);

  myLED.displayWakeup();

  delay(1500);

  myLED.print("0.0123.P5.-");

 */

}

void loop() {

  
}


// End of file.
