#include "NColor.h"

#pragma region RGBA
/// <summary>
/// Default Constructor
/// </summary>
RGBA::RGBA()
	:red(ZERO), green(ZERO), blue(ZERO), alpha(ZERO)
{

}

/// <summary>
/// RGB Constructor
/// </summary>
RGBA::RGBA(RGB rgb)
	:red(rgb.red), green(rgb.green), blue(rgb.blue), alpha(BYTE_MAX)
{

}

/// <summary>
/// RGBA input constructor
/// </summary>
/// <param name="_red">Red</param>
/// <param name="_green">Green</param>
/// <param name="_blue">Blue</param>
/// <param name="_alpha">Alpha</param>
RGBA::RGBA(byte _red, byte _green, byte _blue, byte _alpha)
	:red(_red), green(_green), blue(_blue), alpha(_alpha)
{

}
#pragma endregion

#pragma region RGB
/// <summary>
/// Default Constructor
/// </summary>
RGB::RGB()
	:red(ZERO), green(ZERO), blue(ZERO)
{

}

/// <summary>
/// RGBA Constructor
/// </summary>
RGB::RGB(RGBA rgba)
	:red(rgba.red), green(rgba.green), blue(rgba.blue)
{

}

/// <summary>
/// Input Constructor
/// </summary>
/// <param name="_red">Red</param>
/// <param name="_green">Green</param>
/// <param name="_blue">Blue</param>
RGB::RGB(byte _red, byte _green, byte _blue)
	:red(_red), green(_green), blue(_blue)
{

}
#pragma endregion

#pragma region HSV
/// <summary>
/// Default Constructor
/// </summary>
HSV::HSV()
	:hue(ZERO), saturation(ZERO), value(ZERO)
{

}

/// <summary>
/// Input Constructor
/// </summary>
/// <param name="h">Hue</param>
/// <param name="s">Saturation</param>
/// <param name="v">Value</param>
HSV::HSV(float h, float s, float v)
{
	this->hue = (ZERO <= h && h <= 360) ? h : ZERO;
	this->saturation = (ZERO <= s && s <= 100) ? s : ZERO;
	this->value = (ZERO <= v && v <= 100) ? v : ZERO;
}
#pragma endregion

#pragma region COLOR
/// <summary>
/// Default constructor.
/// </summary>
Color::Color()
	:hexCode(ZERO), rgb(), hsv()
{

}

/// <summary>
/// Hex input constructor.
/// </summary>
/// <param name="_hexCode">Hex code</param>
Color::Color(HEXRGB _hexCode)
	:hexCode(_hexCode), rgb(), hsv()
{
	this->convertHexToRGB();
	this->convertRGBToHSV();
}

/// <summary>
/// HSV Constructor
/// </summary>
/// <param name="_hsv">HSV input</param>
Color::Color(HSV _hsv)
	:hexCode(ZERO), rgb(), hsv(_hsv)
{
	this->convertHSVToRGB();
	this->convertRGBToHex();
}

/// <summary>
/// RGB Constructor
/// </summary>
/// <param name="_rgb">RGB input</param>
Color::Color(RGB _rgb)
	:hexCode(ZERO), rgb(_rgb), hsv()
{
	this->convertRGBToHSV();
	this->convertRGBToHex();
}

void Color::convertHexToRGB()
{
	this->rgb.red = this->hexCode >> 16;
	this->rgb.green = (this->hexCode & 0x00ff00) >> 8;
	this->rgb.blue = (this->hexCode & 0x0000ff);
}

void Color::convertHSVToRGB()
{
	float s = this->hsv.saturation / 100;
	float v = this->hsv.value / 100;
	float C = s * v;
	float X = C * (1 - abs(fmod(this->hsv.hue / 60.0, 2) - 1));
	float m = v - C;
	float r, g, b;

	if (this->hsv.hue >= ZERO && this->hsv.hue < 60) 
    {
		r = C, g = X, b = ZERO;
	}
	else if (this->hsv.hue >= 60 && this->hsv.hue < 120) 
    {
		r = X, g = C, b = ZERO;
	}
	else if (this->hsv.hue >= 120 && this->hsv.hue < 180) 
    {
		r = ZERO, g = C, b = X;
	}
	else if (this->hsv.hue >= 180 && this->hsv.hue < 240) 
    {
		r = ZERO, g = X, b = C;
	}
	else if (this->hsv.hue >= 240 && this->hsv.hue < 300) 
    {
		r = X, g = ZERO, b = C;
	}
	else 
    {
		r = C, g = ZERO, b = X;
	}

	this->rgb.red = (r + m) * 255;
	this->rgb.green = (g + m) * 255;
	this->rgb.blue = (b + m) * 255;
}

void Color::convertRGBToHex()
{
	this->hexCode |= this->rgb.red << 16;
	this->hexCode |= this->rgb.blue << 8;
	this->hexCode |= this->rgb.green;
}

void Color::convertRGBToHSV()
{
	float fCMax = max(max(this->rgb.red, this->rgb.green), this->rgb.blue);
	float fCMin = min(min(this->rgb.red, this->rgb.green), this->rgb.blue);
	float fDelta = fCMax - fCMin;

	if (fDelta > ZERO) 
    {
		if (fCMax == this->rgb.red) 
        {
			this->hsv.hue = 60 * (fmod(((this->rgb.green - this->rgb.blue) / fDelta), 6));
		}
		else if (fCMax == this->rgb.green) 
        {
			this->hsv.hue = 60 * (((this->rgb.blue - this->rgb.red) / fDelta) + 2);
		}
		else if (fCMax == this->rgb.blue) 
        {
			this->hsv.hue = 60 * (((this->rgb.red - this->rgb.green) / fDelta) + 4);
		}

		if (fCMax > ZERO) 
        {
			this->hsv.saturation = fDelta / fCMax;
		}
		else 
        {
			this->hsv.saturation = ZERO;
		}

		this->hsv.value = fCMax;
	}
	else 
    {
		this->hsv.hue = ZERO;
		this->hsv.saturation = ZERO;
		this->hsv.value = fCMax;
	}

	if (this->hsv.hue < ZERO) 
    {
		this->hsv.hue = 360 + this->hsv.hue;
	}
}
#pragma endregion

#pragma region RGBLed
RGBLed::RGBLed(byte _redPin, byte _greenPin, byte _bluePin)
	:redPin(_redPin), greenPin(_greenPin), bluePin(_bluePin), currentColor(RGBA(255, 255, 255, 255))
{
	pinMode(this->redPin, OUTPUT);
	pinMode(this->greenPin, OUTPUT);
	pinMode(this->bluePin, OUTPUT);
}

void RGBLed::write(byte red, byte green, byte blue)
{
	analogWrite(this->redPin, red);
	analogWrite(this->greenPin, green);
	analogWrite(this->bluePin, blue);
}

void RGBLed::setColor(RGB rgb)
{
	this->currentColor.red = rgb.red;
	this->currentColor.green = rgb.green;
	this->currentColor.blue = rgb.blue;
	this->setColor(this->currentColor.red, this->currentColor.green, this->currentColor.blue, this->currentColor.alpha);
}

void RGBLed::setColor(RGBA rgba)
{
	this->currentColor.alpha = rgba.alpha;
	this->currentColor.red = rgba.red;
	this->currentColor.green = rgba.green;
	this->currentColor.blue = rgba.blue;
	this->setColor(this->currentColor.red, this->currentColor.green, this->currentColor.blue, this->currentColor.alpha);
}

void RGBLed::setColor(Color color)
{
	this->currentColor.red = color.rgb.red;
	this->currentColor.green = color.rgb.green;
	this->currentColor.blue = color.rgb.blue;
	this->setColor(this->currentColor.red, this->currentColor.green, this->currentColor.blue, this->currentColor.alpha);
}

void RGBLed::setColor(byte red, byte green, byte blue)
{
	this->currentColor.red = red;
	this->currentColor.green = green;
	this->currentColor.blue = blue;
	this->setColor(this->currentColor.red, this->currentColor.green, this->currentColor.blue, this->currentColor.alpha);
}

void RGBLed::setColor(byte red, byte green, byte blue, byte brightness)
{
	this->currentColor.alpha = brightness;
	this->currentColor.red = red;
	this->currentColor.green = green;
	this->currentColor.blue = blue;
	this->write(map(this->currentColor.red, ZERO, BYTE_MAX, ZERO, this->currentColor.alpha),
				map(this->currentColor.green, ZERO, BYTE_MAX, ZERO, this->currentColor.alpha), 
				map(this->currentColor.blue, ZERO, BYTE_MAX, ZERO, this->currentColor.alpha));
}

void RGBLed::setBrightness(byte brightness)
{
	this->currentColor.alpha = brightness;
	this->setColor(this->currentColor);
}

void RGBLed::off()
{
	this->write(0, 0, 0);
}

void RGBLed::on()
{
	this->setColor(this->currentColor);
}
#pragma endregion