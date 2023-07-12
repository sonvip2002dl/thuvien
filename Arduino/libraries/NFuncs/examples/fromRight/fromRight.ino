#include <NFuncs.h>

/*
    The fromRight() function will add a string all the way to the right of an original string,
    you must define what "all the way to the right is"/maxDisplaySize.
*/

//Serial.begin() rate
int baudrate = 9600;

//String will never be longer than this.
int maxDisplaySize = 32;

//String thats on the left of display.
String option = "LED:";

//Thing we are checking, fictional.
bool ledState = false;

void setup()
{
    Serial.begin(baudrate);
    while (!Serial)
    {
        /* Wait for serial. */
    }

    if (ledState)
    {
        String output = fromRight(option, "On", maxDisplaySize);
        Serial.println(output);
    }
    else
    {
        String output = fromRight(option, "Off", maxDisplaySize);
        Serial.println(output);
    }
}

void loop()
{

}