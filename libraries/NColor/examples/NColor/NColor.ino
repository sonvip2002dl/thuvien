#include <NColor.h>

/*
    This sketch shows how to use NColor for saving, using and converting between RGB, HSV, and HEX Colors.
*/

//Creating your RGB and saving a value.
RGB myRGB = RGB(255, 255, 255);

//Creating you HSV and saving a value.
HSV myHSV = HSV(360, 100, 100);

//Creating your hexcoded and saving a value.
unsigned long hexCode = 0xFFFFFF; //You can use HEXRGB as type if you'd like "HEXRGB hexCode"

//Creating Your color converter.
Color myColor;

void setup()
{
    //Changing your RGB Values.
    myRGB.red = 100;
    myRGB.green = 255;
    myRGB.blue = 0;

    //Changing your HSV Values.
    myHSV.hue = 210;
    myHSV.saturation = 100;
    myHSV.value = 30;

    //Setting your color to your HSV.
    myColor = Color(myHSV);

    //Getting your RGB converted from your HSV.
    myRGB = myColor.rgb;

    //Getting your hexcoded converted from your HSV.
    hexCode = myColor.hexCode;
    
    //Changing your HSV Value.
    myHSV.hue = 300;
    
    //Setting your color to your HSV.
    myColor = Color(myHSV);

    //Getting your RGB converted from your HSV.
    myRGB = myColor.rgb;

    //Getting your hexcoded converted from your HSV.
    hexCode = myColor.hexCode;

    //Setting your hexcoded
    hexCode = 0x00cc00;

    //Setting your color to your hexcoded.
    myColor = Color(hexCode);

    //Getting your HSV converted from hexcoded.
    myHSV = myColor.hsv;
}

void loop()
{
}