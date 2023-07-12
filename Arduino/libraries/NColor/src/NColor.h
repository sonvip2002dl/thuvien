#ifndef NColor
#define NColor

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define HEXRGB_MAX 16777215

#include <NDefs.h>
#include <NFuncs.h>

typedef unsigned long HEXRGB;

typedef struct RGB;

struct RGBA
{
	byte red,
		green,
		blue,
		alpha;
	RGBA();
	RGBA(byte, byte, byte, byte);
	RGBA(RGB);
};

struct RGB
{
	byte red,
		green,
		blue;
	RGB();
	RGB(byte, byte, byte);
	RGB(RGBA);
};

struct HSV
{
	float hue,
		saturation,
		value;
	HSV();
	HSV(float, float, float);
};

struct Color
{
public:
	RGB rgb;
	HSV hsv;
	HEXRGB hexCode;
private:
	void convertHexToRGB();
	void convertHSVToRGB();
	void convertRGBToHex();
	void convertRGBToHSV();
public:
	Color();
	Color(HEXRGB);
	Color(HSV);
	Color(RGB);
};

class RGBLed
{
public:
	RGBLed(byte, byte, byte);
	void setColor(RGB);
	void setColor(RGBA);
	void setColor(Color);
	void setColor(byte, byte, byte);
	void setColor(byte, byte, byte, byte);
	void setBrightness(byte);
	void on();
	void off();
	RGBA currentColor;
private:
	void write(byte, byte, byte);
	const byte redPin;
	const byte greenPin;
	const byte bluePin;
};
#endif