/*
    ====Use typed byte array to manipulate vars through bytes or to get bytes====

    -Uses template to use any type.
    -Cannot be used on dynamic types.
    -Can be initialized using type or byte pointer to array or not initialized if type is a pointer(Only useful for char *).
    -Implicit conversion operators.
    -All members are public.
*/

#include <TypedByteArray.h>

#define BAUDRATE 9600

const byte byteArray[4] = { 230, 7, 0, 0 }; //2022
auto number = TypedByteArray<unsigned long, sizeof(unsigned long)>(byteArray); //<- Use an array to init.

//                 Type           Size in bytes
//                  |                   |
//TypedByteArray<unsigned long, sizeof(unsigned long)> number = TypedByteArray<unsigned long, sizeof(unsigned long)>(2022);

//Alternative syntax.
//auto number = TypedByteArray<unsigned long, sizeof(unsigned long)>(2022);


void printUnsingedLongTypedByteArray(TypedByteArray<unsigned long, sizeof(unsigned long)> tba)
{
    Serial.println(tba); //implicit conversion to unsigned long.
    //Serial.println((unsigned long)number); explicit conversion.

    //Getting the bytes. They come in low byte first.
    byte *tbaBytes = tba; //implicit conversion.
    //byte *numberBytes = (byte *)number; //explicit conversion to get pointer to byte array.

    for (int i = 0; i < sizeof(unsigned long); i++)
    {
        String thisLine = "Byte[";
        thisLine += i;
        thisLine += "]: ";
        thisLine += tbaBytes[i];

        Serial.println(thisLine);
    }
}

void setup()
{
    Serial.begin(BAUDRATE);
    while (!Serial);
    delay(200);

    Serial.println("Initial value:");
    printUnsingedLongTypedByteArray(number);

    Serial.println("\nNext value:");
    number = 2021; //implicit cast operator to type(unsigned long in this case). Can also use Byte * to array implicitly.
    printUnsingedLongTypedByteArray(number);

    Serial.println("\nNext value:");
    number.data = 2020; //Direct access to members.
    printUnsingedLongTypedByteArray(number);
    
    Serial.println("\nLast value:");
    number = 1431655765; //Repeating 01 in binary.
    printUnsingedLongTypedByteArray(number);
}

void loop()
{
    _NOP(); // DO NOTHING!
}