#include <NDefs.h>

#define BAUDRATE 9600

void setup()
{
    Serial.begin(BAUDRATE);
    while (!Serial)
    {

    }
    delay(1000);
    Serial.print("NDefs Version ");
    Serial.print(NDEFS_MAJOR);
    Serial.print('.');
    Serial.print(NDEFS_MINOR);
    Serial.print('.');
    Serial.print(NDEFS_PATCH);
}

void loop()
{

}