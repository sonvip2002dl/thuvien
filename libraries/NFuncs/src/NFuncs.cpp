#include <NFuncs.h>

#pragma region x2i
/// <summary>
/// Outputs unsinged long from Hex Char array.
/// </summary>
/// <param name="s">: Char Array</param>
/// <param name="len">: Length of characters</param>
/// <returns>Number From Hex</returns>
unsigned long x2i(char* s, uint8_t len)
{
	unsigned long x = ZERO;
	uint8_t iter = ZERO;
	while (iter < len)
	{
		char c = *s;
		if (c >= '0' && c <= '9')
		{
			x *= 16;
			x += c - '0';
		}
		else if (c >= 'A' && c <= 'F')
		{
			x *= 16;
			x += (c - 'A') + 10;
		}
		else break;
		s++;
		iter++;
	}
	return x;
}

/// <summary>
/// Outputs unsinged long from Hex Char array.
/// </summary>
/// <param name="s">: Char Array</param>
/// <returns>Number From Hex</returns>
unsigned long x2i(char* s)
{
	unsigned long x = ZERO;
	for (;;)
	{
		char c = *s;
		if (c >= '0' && c <= '9')
		{
			x *= 16;
			x += c - '0';
		}
		else if (c >= 'A' && c <= 'F')
		{
			x *= 16;
			x += (c - 'A') + 10;
		}
		else break;
		s++;
	}
	return x;
}
#pragma endregion

#pragma region toHex
/// <summary>
/// Outputs string from unsigned long input and string length, replaces empty spots with 0's.
/// </summary>
/// <param name="input">: Long number</param>
/// <param name="stringLength">: End Length</param>
/// <returns>End string result</returns>
String toHex(unsigned long input, byte stringLength)
{
	String toggled = String(input, HEX);
	toggled.toUpperCase();
	if (toggled.length() != stringLength)
	{
		byte outLength = toggled.length();
		String original = toggled;
		toggled = "";
		for (int i = ZERO; i < stringLength - outLength; i++)
		{
			toggled += "0";
		}
		toggled += original;
	}
	return toggled;
}
#pragma endregion

#pragma region octalToDecimal
/// <summary>
/// Returns base-10 from octal input.
/// </summary>
/// <param name="n">: Octal Number</param>
/// <returns>int (Base-10)</returns>
static int octalToDecimal(int n)
{
	int num = n;
	int dec_value = ZERO;
	int base = 1;
	int temp = num;
	while (temp > ZERO) {
		int last_digit = temp % 10;
		temp = temp / 10;
		dec_value += last_digit * base;
		base = base * 8;
	}
	return dec_value;
}
#pragma endregion

#pragma region bitCast
/// <summary>
/// Cast ulong to long with correct signage
/// </summary>
/// <param name="in"></param>
/// <returns>value</returns>
long bitCast(unsigned long in)
{
	return (in - LONG_MAX);
}
#pragma endregion

#pragma region bitCast
/// <summary>
/// Cast long to ulong with correct signage
/// </summary>
/// <param name="in"></param>
/// <returns>value</returns>
unsigned long bitCast(long in)
{
	return (in + LONG_MAX);
}
#pragma endregion

#pragma region mapf
/// <summary>
/// Map values double
/// </summary>
/// <param name="x">Value to map</param>
/// <param name="in_min">Input min</param>
/// <param name="in_max">Input max</param>
/// <param name="out_min">Output min</param>
/// <param name="out_max">Output max</param>
/// <returns>mapped values</returns>
double mapf(double x, double in_min, double in_max, double out_min, double out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#pragma endregion

#pragma region HornerScheme
/// <summary>
/// The Horner Scheme
/// </summary>
/// <param name="Num"></param>
/// <param name="Divider"></param>
/// <param name="Factor"></param>
/// <returns></returns>
unsigned long hornerScheme(unsigned long Num, unsigned long Divider, unsigned long Factor)
{
	unsigned long Remainder = ZERO, Quotient = ZERO, Result = ZERO;
	Remainder = Num % Divider;
	Quotient = Num / Divider;
	if (!(Quotient == ZERO && Remainder == ZERO))
		Result += hornerScheme(Quotient, Divider, Factor) * Factor + Remainder;
	return Result;
}
#pragma endregion

#pragma region intToFreq
/// <summary>
/// 4-Digit to xxx.xxx
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
double intToFreq(double input)
{
	input += 10000;
	input /= 100;
	return input;
}
#pragma endregion

#pragma region boolToString
/// <summary>
/// Converts bool to string "True"/"False"
/// </summary>
/// <param name="input">true/false input</param>
/// <returns>true/false string</returns>
String boolToString(int input)
{
	if (input > ZERO)
	{
		return "True";
	}
	else
	{
		return "False";
	}
}
#pragma endregion

#pragma region boolToString
/// <summary>
/// Converts bool to string "True"/"False"
/// </summary>
/// <param name="input">true/false input</param>
/// <returns>true/false string</returns>
String boolToString(bool input)
{
	if (input)
	{
		return "True";
	}
	else
	{
		return "False";
	}
}
#pragma endregion

#pragma region byteWrite
void byteWrite(byte pin, byte byteOut) {
	for (byte i = ZERO; i < 8; i++) 
	{
		digitalWrite(pin, LOW);
		if (0x80 & byteOut) digitalWrite(pin, HIGH);
		else digitalWrite(pin, LOW);
		digitalWrite(pin, HIGH);
		digitalWrite(pin, LOW);
		byteOut <<= 1;
	}
}
#pragma endregion

#pragma region printRight
String fromRight(String original, String right, byte length)
{
  byte originalLength = original.length();
  byte rightLength = right.length();
  byte count = length - originalLength - rightLength;
  for (byte i = 0; i < count; i++)
  {
    original += ' ';
  }
  original += right;
  return original;
}
#pragma endregion