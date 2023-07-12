#include <NDefs.h> //<- Library

/*
    The isBetween(val, low, high) defeniton exists to make your code just that one bit more
    readable.
*/

//Serial.begin() baudrate
int baudrate = 9600;

//Potentiometer pin.
int potentiometer = A0;

//Low value for comparison.
const int low = 32;

//High value for comparison.
const int high = 800;

//Potentiometer positon.
int value;

void setup()
{
    Serial.begin(baudrate);
}

void loop()
{
    //Capturing potentiometer position.
    value = analogRead(potentiometer);

    //Checking if its between our determind low/high.
    if (isBetween(value, low, high))
    {
        //Print that value is in between.
        Serial.print("Value is between: ");
        Serial.println(value);
    }
}