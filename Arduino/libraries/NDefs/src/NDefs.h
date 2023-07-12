#ifndef NDefs_h
#define NDefs_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#define NDEFS_VERSION "1.1.0"
#define NDEFS_MAJOR 1
#define NDEFS_MINOR 1
#define NDEFS_PATCH 0

#define ZERO 0
#define NEWLINE '\n'
#define CARRIAGERETURN '\r'
#define NULLTERMINATOR '\0'

#ifdef ARDUINO_ARCH_AVR
    #define ULONG_MAX 4294967295
    #define LONG_MAX 2147483647
    #define LONG_MIN -2147483648

    #define UINT_MAX 65535
    #define INT_MAX 32767
    #define INT_MIN -32768

    #define BYTE_MAX 255

    #define ADC_MAX 1023
    #define PWN_MAX 255

    #define AVR_RESET() ((void (*)(void))0x00)()
#endif

#define SERIALCOM_BAUD 1000000

#define reinterpret_c_style(type, origin) *((type *)origin)
#define isBetween(val, low, high) (low < val && val < high) ? true : false

#define NOLOOP void loop() { }
#define addInterrupt(pin, ISR, mode) attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);
#define removeInterrupt(pin) detachInterrupt(digitalPinToInterrupt(pin));

#endif