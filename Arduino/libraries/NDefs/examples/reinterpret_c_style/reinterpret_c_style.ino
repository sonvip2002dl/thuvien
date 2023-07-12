//Only need the defenition for this example.
#include <NDefs.h>

//Declaring a byte array to change to a uint32_t/unsigned int
byte byteArray[4] = { 0x41, 0xAE, 0x01, 0xF2 };

//Declaring a uint32_t/unsigned int to get the byte array as an integer.
uint32_t myNumber;

void setup()
{
    //C-Style cast.
    myNumber = *((unsigned long*)byteArray);
    
    //Changing values.
    byteArray[3] = 0x00;
    
    //using defenition.
    myNumber = reinterpret_c_style(uint32_t, byteArray);
    //                                 ^         ^
    //                      Destination type | Source
}

void loop()
{

}