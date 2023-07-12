#include <NFuncs.h>

//Serial.begin() baudrate.
int baudrate = 9600;

//mapped value.
float newValue;

void setup()
{
    Serial.begin(baudrate);
}

void loop()
{
    //Reading potentiometer pin.
    int tempValue = analogRead(A1);

    //mapping a float.
    newValue = mapf(tempValue, 0, 1023, 0, 100);

    //Printing mapped value.
    Serial.print(newValue);
    Serial.println('%');
}