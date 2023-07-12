#include <NFuncs.h>

//Serial.begin() baudrate.
int baudrate = 9600;

//Your bool.
bool myBool = false;

void setup()
{
    Serial.begin(baudrate);
    
    while (!Serial)
    {
        //Waiting for Serial.
    }

    //Printing out True/False String.
    Serial.println(boolToString(myBool));
    
    //Changing your bool state.
    myBool = true;

    String myString = boolToString(myBool);

    //Printing out your string.
    Serial.println(myString);
}

void loop()
{

}