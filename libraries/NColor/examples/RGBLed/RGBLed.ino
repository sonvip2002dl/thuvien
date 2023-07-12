#include <NColor.h>

const int waitTime = 1000;

//LED red pin.
int redLEDPin = 9;

//LED green pin.
int greenLEDPin = 10;

//LED blue pin.
int blueLEDPin = 11;

//RGBLed constructor.
RGBLed led = RGBLed(redLEDPin, greenLEDPin, blueLEDPin);
//      ^               ^            ^           ^
// your instance     red pin    green pin    blue pin

//A nice color at half % alpha.
RGBA niceColor(102, 255, 71, 127);

//Saving your first favorite color as RGB.
RGB myFavoriteColor(127, 255, 0);

//Saving your second favorite color as a HEX coded color, just an unsigned long.
HEXRGB mySecondFavoriteColor = 0x126DE3;

//Saving your third favorite color as a HSV.
HSV myThirdFavoriteColor(65, 100, 90);

//Creating a new instance of the Color struct which is used for converting colors to other types(RGB, HEXRGB, HSV).
Color colorConverter;

void setup()
{
    //Setting color using RGB defaulting brightness to max. You can also use RGBA to include brightness.
    led.setColor(RGB(127, 255, 63));
}

void loop()
{
    //Setting color using red, green, and blue values.
    led.setColor(255, 0, 0);
    delay(waitTime);
    led.setColor(0, 255, 0);
    delay(waitTime);
    led.setColor(0, 0, 255);
    delay(waitTime);
    led.setColor(255, 255, 255);
    delay(waitTime);

    //Setting color using red, green, blue, and brightness values.
    led.setColor(255, 255, 255, 127);
    delay(waitTime);
    led.setColor(255, 255, 255, 63);
    delay(waitTime);

    //Going back to white at last brightness.
    led.setColor(255, 255, 255);
    delay(waitTime);

    //Going back to white at max brightness.
    led.setBrightness(255);
    led.setColor(255, 255, 255);
    delay(waitTime);

    //Setting a nice color.
    led.setColor(niceColor);
    delay(waitTime);

    //Setting the color to your favorite color at last brightness.
    led.setColor(myFavoriteColor);
    delay(waitTime);

    //Setting brightness back to max.
    led.setBrightness(255);

    //Setting the color as your second favorite color by using Color struct.
    led.setColor(Color(mySecondFavoriteColor));
    delay(waitTime);

    //Using Color to convert from HSV and saving RGB, HEXRGB and HSV into colorConverter.
    colorConverter = Color(myThirdFavoriteColor);

    //Setting the color as your third favorite color after converting from HSV to RGB.
    led.setColor(colorConverter.rgb);
    delay(waitTime);
}