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

EEPROMsimple EEPROM;       // initialize an instance of this class

/*******************  Create some dummy data to read and space to write  ******************/
byte a[20] = {"abcdefghijklmnopqrs"};         // array data to write
int b[5] = {256,257,258,259,260};             // array data to write
unsigned int c[5] = {32768,32769,32770,32771,32772}; // array data to write
long d[5] = {65536,65537,65538,65539,65540};
unsigned long e[5] = {2147483648,2147483649,2147483650,2147483651,2147483652};
float f[5] = {3.14,1.59,2.67,8.45,9.99};

byte read_data_a[20];                         // array to hold data read from memory
int read_data_b[5];                           // array to hold data read from memory
unsigned int read_data_c[5];                  // array to hold data read from memory
long read_data_d[5];                          // array to hold data read from memory
unsigned long read_data_e[5];                 // array to hold data read from memory
float read_data_f[5];                         // array to hold data read from memory

void setup()
{
  uint32_t address = 0;                       // create a 32 bit variable to hold the address (uint32_t=long)
  byte value;                                 // create variable to hold the data value read
  byte data;                                  // create variable to hold the data value sent
  Serial.begin(9600);                         // set communication speed for the serial monitor
  SPI.begin();                                // start communicating with the memory chip
    
/************  Write a Sequence of Bytes from an Array *******************/
  Serial.println("\nWriting byte array using Sequential: ");
  EEPROM.WriteByteArray(0, a, sizeof(a));        // Write array e to memory starting at address 0

/************ Read a Sequence of Bytes from Memory into an Array **********/
  Serial.println("Reading byte array using sequential: ");
  EEPROM.ReadByteArray(0, read_data_a, sizeof(read_data_a));   // Read array into read_data_a starting at address 0
  for(int i=0; i<sizeof(read_data_a); i++){     // print the array
    Serial.println((char)read_data_a[i]);       // We need to cast it as a char
  }                                             // to make it print as a character

/************  Write an Integer Array *******************/
  Serial.println("\nWriting integer array using Sequential: ");
  EEPROM.WriteIntArray(0, b, 5);                  // Write array b to memory starting at address 0
                                               
/************ Read an Integer Array **********/
  Serial.println("Reading integer array using sequential: ");
  EEPROM.ReadIntArray(0, read_data_b, 5);   // Read from memory into empty array read_data_b
  for(int i=0; i<5; i++){                 // Output read data to serial monitor
    Serial.println(read_data_b[i]);         
  } 

/************  Write an Unsigned Integer Array *******************/
  Serial.println("\nWriting unsigned integer array using Sequential: ");
  EEPROM.WriteUnsignedIntArray(0, c, 5);     // Use the array of characters defined in z above
                                               // write to memory starting at address 0
/************ Read an Unsigned Integer Array **********/
  Serial.println("Reading unsigned integer array using sequential: ");
  EEPROM.ReadUnsignedIntArray(0, read_data_c, 5);  // Read from memory into empty array read_data_c
  for(int i=0; i<5; i++){                        // Output read data to serial monitor
    Serial.println(read_data_c[i]);         
  } 

/************  Write a Long Array *******************/
  Serial.println("\nWriting long array: ");
  EEPROM.WriteLongArray(0, d, 5);                  // Write array d to memory starting at address 0

/************ Read a Long Array **********/
  Serial.println("Reading long array: ");
  EEPROM.ReadLongArray(0, read_data_d, 5);       // Read from memory into empty array read_data_d
  for(int i=0; i<5; i++){                      // Output read data to serial monitor
    Serial.println(read_data_d[i]);         
  } 

/************  Write an Unsigned Long Array *******************/
  Serial.println("\nWriting unsigned long array: ");
  EEPROM.WriteUnsignedLongArray(0, e, 5);        // Write array e to memory starting at address 0

/************ Read an Unsigned Long Array **********/
  Serial.println("Reading unsigned long array: ");
  EEPROM.ReadUnsignedLongArray(0, read_data_e, 5);   // Read from memory into empty array read_data_e
  for(int i=0; i<5; i++){                          // Output read data to serial monitor
    Serial.println(read_data_e[i]);         
  }

/************  Write a Float Array *******************/
  Serial.println("\nWriting float array: ");
  EEPROM.WriteFloatArray(0, f, 5);                 // Write array f to memory starting at address 0

/************ Read Float Array **********/
  Serial.println("Reading float array: ");
  EEPROM.ReadFloatArray(0, read_data_f, 5);       // Read from memory into empty array read_data_f
  for(int i=0; i<5; i++){                       // Output read data to serial monitor
    Serial.println(read_data_f[i]);         
  }
}

void loop()
{
}
