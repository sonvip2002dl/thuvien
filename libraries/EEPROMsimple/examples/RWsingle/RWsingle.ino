/* Example program for use with EEPROMsimple.h 
   Arduino Uno Memory Expansion Sample Program
   Author:  J. B. Gallaher       07/09/2016
   Library created and expanded by: D. Dubins 23-Jan-20

   Sample program to use a Serial EEPROM chip to expand memory for an Arduino Uno
   giving access to an additional 128kB of random access memory.  The 25LC1024 uses
   the Serial Peripheral Interface (SPI) to transfer data and commands between the
   UNO and the memory chip.

   Used the following components:
   (1) Arduino Uno
   (2) Microchip 25LC1024 SPI EEPROM chip
   
   Wiring:
   25LC1024 - Uno:
   ---------------
   pin 1 -- D10 (SS)
   pin 2 -- D12 (MISO)
   pin 3, 7, 8 -- +5V
   pin 4 -- GND
   pin 5 -- D11 (MOSI)
   pin 6 -- D13 (SCK)
*/
#include <EEPROMsimple.h>

#define CSPIN 10       // Default Chip Select Line for Uno (change as needed)
EEPROMsimple EEPROM;       //initialize an instance of this class

/*******  Set up code to define variables and start the SCI and SPI serial interfaces  *****/
void setup()
{
  uint32_t address = 0;                       // create a 32 bit variable to hold the address (uint32_t=long)
  Serial.begin(9600);                         // set communication speed for the serial monitor
  SPI.begin();                                // start communicating with the memory chip
    
  // And now the fun begins:
  /**********Write a Single Byte *******************/
  byte data = 0;                              // initialize the data
  for(int i = 0; i <=5; i++){                 // Let's write 5 individual bytes to memory 
    address = i;                              // use the loop counter as the address
    EEPROM.WriteByte(address, data);            // now write the data to that address
    data+=2;                                  // increment the data by 2 
  }

/********* Read a single Byte *********************/
  Serial.println("Reading each data byte individually: ");
  byte value;                                 // create variable to hold the data value read
  for(int i = 0; i <=5; i++){                 // start at memory location 0 and end at 5
    address = i;                              // use the loop counter as the memory address
    value = EEPROM.ReadByte(address);           // reads a byte of data at that memory location
    Serial.println(value);                    // Let's see what we got
  }

/************  Write an Integer *******************/
  Serial.println("\nWriting integer using sequential: ");
  int tempInt1=-32768;                         // highest integer # is 32767
  EEPROM.WriteInt(0, tempInt1);                  // send tempInt1 to EEPROM starting from address 0
 
/************ Read an Integer from Memory into an Int **********/
  Serial.println("Reading integer using sequential: ");
  int tempInt2=EEPROM.ReadInt(0);                // Read integer from memory address 0
  Serial.println(tempInt2);                    // print as integer

/************  Write an Unsigned Integer *******************/
  Serial.println("\nWriting unsigned integer using sequential: ");
  unsigned int tempUnsignedInt1=65535;         // highest unsigned integer is 65535
  EEPROM.WriteInt(0, tempUnsignedInt1);          // send tempUnsignedInt1 to EEPROM starting from address 0
 
/************ Read an Unsigned Integer from Memory into an Int **********/
  Serial.println("Reading unsigned integer using sequential: ");
  unsigned int tempUnsignedInt2=EEPROM.ReadUnsignedInt(0);     // Read integer from memory address 0
  Serial.println(tempUnsignedInt2);            // print as unsigned integer

/************  Write Long *******************/
  Serial.println("\nWriting long using sequential: ");
  long tempLong1=2147483647;                   // highest long #
  EEPROM.WriteLong(0, tempLong1);                // send tempLong1 to EEPROM starting from address 0

/************ Read Long from Memory **********/
  Serial.println("Reading long using sequential: ");
  long tempLong2=EEPROM.ReadLong(0);             // Read long from memory address 0
  Serial.println(tempLong2);                   // print as long

/************  Write Unsigned Long *******************/
  Serial.println("\nWriting unsigned long using sequential: ");
  long tempUnsignedLong1=4294967295;           // highest unsigned long #
  EEPROM.WriteUnsignedLong(0, tempUnsignedLong1);        // send tempUnsignedLong1 to EEPROM starting from address 0

/************ Read Unsigned Long from Memory **********/
  Serial.println("Reading unsigned long using sequential: ");
  unsigned long tempUnsignedLong2=EEPROM.ReadUnsignedLong(0);  // Read unsigned long from memory address 0
  Serial.println(tempUnsignedLong2);                   // print as unsigned long

/************  Write a Float using Sequential *******************/
  Serial.println("\nWriting float using sequential: ");
  float tempFloat1=3.141593;                   // a float #
  EEPROM.WriteFloat(0, tempFloat1);              // send tempFloat1 to EEPROM starting at address 0

/************ Read Float from Memory into a Float **********/
  Serial.println("Reading float using sequential: ");
  float tempFloat2=EEPROM.ReadFloat(0);           // Read float from memory address 0
  Serial.println(tempFloat2,6);                 // print as float
}

void loop(){
}
