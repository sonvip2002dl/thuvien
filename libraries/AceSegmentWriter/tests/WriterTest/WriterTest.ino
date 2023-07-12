#line 2 "WriterTest.ino"

/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include <stdarg.h>
#include <Arduino.h>
#include <AUnit.h>
#include <AceSegment.h>
#include <ace_segment/testing/TestableLedModule.h>
#include <AceSegmentWriter.h>

using aunit::TestRunner;
using aunit::TestOnce;
using namespace ace_segment;
using namespace ace_segment::testing;

const uint8_t NUM_DIGITS = 4;

// TestableModule allocates 1 more byte than NUM_DIGITS to test buffer overflow.
TestableLedModule<NUM_DIGITS> ledModule;
NumberWriter<LedModule> numberWriter(ledModule);
ClockWriter<LedModule> clockWriter(ledModule);
TemperatureWriter<LedModule> temperatureWriter(ledModule);
CharWriter<LedModule> charWriter(ledModule);
StringWriter<LedModule> stringWriter(charWriter);

// ----------------------------------------------------------------------
// Tests for CharWriter.
// ----------------------------------------------------------------------

class CharWriterTest : public TestOnce {
  protected:
    void setup() override {
      charWriter.clear();
      mPatterns = ledModule.getPatterns();
    }

    uint8_t* mPatterns;
};

testF(CharWriterTest, writeAt) {
  charWriter.writeCharAt(0, '0');
  assertEqual(kPattern0, mPatterns[0]);
}

testF(CharWriterTest, writeAt_outOfBounds) {
  mPatterns[4] = 0;

  charWriter.writeCharAt(4, 'a');
  assertEqual(0, mPatterns[4]);
}

// ----------------------------------------------------------------------
// Tests for StringWriter.
// ----------------------------------------------------------------------

class StringWriterTest : public TestOnce {
  protected:
    void setup() override {
      stringWriter.clear();
      mPatterns = ledModule.getPatterns();
    }

    void assertPatternsEqual(int n, ...) {
      va_list args;
      va_start(args, n);
      for (int i = 0; i < n; i++) {
        uint8_t pattern = va_arg(args, int);
        assertEqual(pattern, mPatterns[i]);
      }
      va_end(args);
    }

    uint8_t* mPatterns;
};

testF(StringWriterTest, writeStringAt) {
  stringWriter.writeStringAt(0, ".1.2.3");

  // Should be (".", "1.", "2.", "3") as the 4 digits
  assertPatternsEqual(
    4,
    kPatternSpace | 0x80,
    kPattern1 | 0x80,
    kPattern2 | 0x80,
    kPattern3
  );
}

// ----------------------------------------------------------------------
// Tests for NumberWriter.
// ----------------------------------------------------------------------

class NumberWriterTest : public TestOnce {
  protected:
    void setup() override {
      numberWriter.clear();
      mPatterns = ledModule.getPatterns();
    }

    uint8_t* mPatterns;
};

testF(NumberWriterTest, writeHexCharAt) {
  numberWriter.writeHexCharAt(0, 0);
  assertEqual(kPattern0, mPatterns[0]);

  numberWriter.writeHexCharAt(1, 0);
  numberWriter.writeDecimalPointAt(1);
  assertEqual(kPattern0 | 0x80, mPatterns[1]);

  numberWriter.writeDecimalPointAt(1, false);
  assertEqual(kPattern0, mPatterns[1]);
}

testF(NumberWriterTest, writeHexCharAt_invalid_char_becomes_space) {
  numberWriter.writeHexCharAt(0, 255);
  assertEqual(kPatternSpace, mPatterns[0]);
}

testF(NumberWriterTest, writeHexCharAt_outOfBounds_writes_nothing) {
  mPatterns[4] = 0;  // TestableLedModule allocates an extra byte to test this

  numberWriter.writeHexCharAt(4, kHexCharMinus);
  numberWriter.writeDecimalPointAt(4);
  assertEqual(0, mPatterns[4]);
}

testF(NumberWriterTest, writeHexChars2At) {
  numberWriter.writeHexChars2At(0, 3, 4);

  assertEqual(kPattern3, mPatterns[0]);
  assertEqual(kPattern4, mPatterns[1]);
}

testF(NumberWriterTest, writeHexChars2At_invalid_char_becomes_space) {
  numberWriter.writeHexChars2At(0, 3, 255);

  assertEqual(kPattern3, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
}

testF(NumberWriterTest, writeHexCharsAt) {
  const hexchar_t CHARS[] = {2, 3};
  numberWriter.writeHexCharsAt(0, CHARS, 2);

  assertEqual(kPattern2, mPatterns[0]);
  assertEqual(kPattern3, mPatterns[1]);
}

testF(NumberWriterTest, writeBcd2At) {
  numberWriter.writeBcd2At(0, 0x12);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);

  numberWriter.writeBcd2At(2, 0x34);
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);
}

testF(NumberWriterTest, writeDec2At_pad_zero) {
  numberWriter.writeDec2At(0, 1);
  assertEqual(kPattern0, mPatterns[0]);
  assertEqual(kPattern1, mPatterns[1]);

  numberWriter.writeDec2At(0, 12);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);

  // Number is too big, so 2 spaces are written.
  numberWriter.writeDec2At(0, 123, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
}

testF(NumberWriterTest, writeDec2At_pad_space) {
  numberWriter.writeDec2At(0, 1, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPattern1, mPatterns[1]);

  numberWriter.writeDec2At(0, 12, kPatternSpace);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);

  // Number is too big, so 2 spaces are written.
  numberWriter.writeDec2At(0, 123, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
}

testF(NumberWriterTest, writeDec4At_pad_zero) {
  numberWriter.writeDec4At(0, 1);
  assertEqual(kPattern0, mPatterns[0]);
  assertEqual(kPattern0, mPatterns[1]);
  assertEqual(kPattern0, mPatterns[2]);
  assertEqual(kPattern1, mPatterns[3]);

  numberWriter.writeDec4At(0, 12);
  assertEqual(kPattern0, mPatterns[0]);
  assertEqual(kPattern0, mPatterns[1]);
  assertEqual(kPattern1, mPatterns[2]);
  assertEqual(kPattern2, mPatterns[3]);

  numberWriter.writeDec4At(0, 123);
  assertEqual(kPattern0, mPatterns[0]);
  assertEqual(kPattern1, mPatterns[1]);
  assertEqual(kPattern2, mPatterns[2]);
  assertEqual(kPattern3, mPatterns[3]);

  numberWriter.writeDec4At(0, 1234);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);

  // Number is too big, so 4 spaces are written.
  numberWriter.writeDec4At(0, 12345);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
  assertEqual(kPatternSpace, mPatterns[2]);
  assertEqual(kPatternSpace, mPatterns[3]);
}

testF(NumberWriterTest, writeDec4At_pad_space) {
  numberWriter.writeDec4At(0, 1, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
  assertEqual(kPatternSpace, mPatterns[2]);
  assertEqual(kPattern1, mPatterns[3]);

  numberWriter.writeDec4At(0, 12, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
  assertEqual(kPattern1, mPatterns[2]);
  assertEqual(kPattern2, mPatterns[3]);

  numberWriter.writeDec4At(0, 123, kPatternSpace);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPattern1, mPatterns[1]);
  assertEqual(kPattern2, mPatterns[2]);
  assertEqual(kPattern3, mPatterns[3]);

  numberWriter.writeDec4At(0, 1234, kPatternSpace);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);

  // Number is too big, so 4 spaces are written.
  numberWriter.writeDec4At(0, 12345);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
  assertEqual(kPatternSpace, mPatterns[2]);
  assertEqual(kPatternSpace, mPatterns[3]);
}

testF(NumberWriterTest, writeHexByteAt) {
  numberWriter.writeHexByteAt(0, 0x1F);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPatternF, mPatterns[1]);
}

testF(NumberWriterTest, writeHexWordAt) {
  numberWriter.writeHexWordAt(0, 0x12AB);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);
  assertEqual(kPatternA, mPatterns[2]);
  assertEqual(kPatternB, mPatterns[3]);
}

testF(NumberWriterTest, writeUnsignedDecimalAt) {
  uint8_t written = numberWriter.writeUnsignedDecimalAt(0, 123);
  assertEqual(3, written);
  assertEqual(kPattern1, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);
  assertEqual(kPattern3, mPatterns[2]);

  // Even if it goes off the end of the LED module, the return value is the
  // number of characters that would have been written if the LED module was
  // long enough.
  written = numberWriter.writeUnsignedDecimalAt(3, 123);
  assertEqual(3, written);
}

testF(NumberWriterTest, writeUnsignedDecimalAt_boxed) {
  uint8_t written = numberWriter.writeUnsignedDecimalAt(0, 34, 4);
  assertEqual(4, written);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternSpace, mPatterns[1]);
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);

  written = numberWriter.writeUnsignedDecimalAt(0, 34, 2);
  assertEqual(2, written);
}

testF(NumberWriterTest, writeSignedDecimalAt) {
  uint8_t written = numberWriter.writeSignedDecimalAt(0, -23);
  assertEqual(3, written);
  assertEqual(kPatternMinus, mPatterns[0]);
  assertEqual(kPattern2, mPatterns[1]);
  assertEqual(kPattern3, mPatterns[2]);
}

testF(NumberWriterTest, writeSignedDecimalAt_boxed) {
  uint8_t written = numberWriter.writeSignedDecimalAt(0, -12, 4);
  assertEqual(4, written);
  assertEqual(kPatternSpace, mPatterns[0]);
  assertEqual(kPatternMinus, mPatterns[1]);
  assertEqual(kPattern1, mPatterns[2]);
  assertEqual(kPattern2, mPatterns[3]);
}

// ----------------------------------------------------------------------
// Tests for ClockWriter.
// ----------------------------------------------------------------------

class ClockWriterTest: public TestOnce {
  protected:
    void setup() override {
      clockWriter.clear();
      mPatterns = ledModule.getPatterns();
    }

    uint8_t* mPatterns;
};

testF(ClockWriterTest, writeColon) {
  clockWriter.numberWriter().writeDec2At(0, 12);
  clockWriter.numberWriter().writeDec2At(2, 34);

  // no colon by default
  assertEqual(kPattern2, mPatterns[1]);

  // turn on colon
  clockWriter.writeColon(true); // turns it on
  assertEqual(kPattern2 | 0x80, mPatterns[1]);
}

testF(ClockWriterTest, writeHourMinute12) {
  clockWriter.writeHourMinute12(2, 34);
  assertEqual(kPatternSpace, mPatterns[0]); // space
  assertEqual(kPattern2 | 0x80, mPatterns[1]); // colon on by default
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);
}

testF(ClockWriterTest, writeHourMinute24) {
  clockWriter.writeHourMinute24(2, 34);
  assertEqual(kPattern0, mPatterns[0]); // zero
  assertEqual(kPattern2 | 0x80, mPatterns[1]); // colon on by default
  assertEqual(kPattern3, mPatterns[2]);
  assertEqual(kPattern4, mPatterns[3]);
}

// ----------------------------------------------------------------------
// Tests for TemperatureWriter.
// ----------------------------------------------------------------------

class TemperatureWriterTest: public TestOnce {
  protected:
    void setup() override {
      temperatureWriter.clear();
      mPatterns = ledModule.getPatterns();
    }

    uint8_t* mPatterns;
};

testF(TemperatureWriterTest, writeTempDegAt) {
  uint8_t written = temperatureWriter.writeTempDegAt(0, -9);
  assertEqual(3, written);
  assertEqual(kPatternMinus, mPatterns[0]); // -
  assertEqual(kPattern9, mPatterns[1]); // 9
  assertEqual(kPatternDeg, mPatterns[2]); // deg

  written = temperatureWriter.writeTempDegAt(0, -9 /*temp*/, 4 /*boxSize*/);
  assertEqual(4, written);
  assertEqual(kPatternSpace, mPatterns[0]); // space
  assertEqual(kPatternMinus, mPatterns[1]); // -
  assertEqual(kPattern9, mPatterns[2]); // 9
  assertEqual(kPatternDeg, mPatterns[3]); // deg
}

testF(TemperatureWriterTest, writeTempDegCAt) {
  uint8_t written = temperatureWriter.writeTempDegCAt(
      0, -9 /*temp*/, 4 /*boxSize*/);
  assertEqual(4, written);
  assertEqual(kPatternMinus, mPatterns[0]); // -
  assertEqual(kPattern9, mPatterns[1]); // 9
  assertEqual(kPatternDeg, mPatterns[2]); // deg
  assertEqual(kPatternC, mPatterns[3]); // C
}

testF(TemperatureWriterTest, writeTempDegFAt) {
  uint8_t written = temperatureWriter.writeTempDegFAt(
      0, -9 /*temp*/, 4 /*boxSize*/);
  assertEqual(4, written);
  assertEqual(kPatternMinus, mPatterns[0]); // -
  assertEqual(kPattern9, mPatterns[1]); // 9
  assertEqual(kPatternDeg, mPatterns[2]); // deg
  assertEqual(kPatternF, mPatterns[3]); // F
}

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // Wait for stability on some boards, otherwise garage on Serial
#endif

  Serial.begin(115200); // ESP8266 default of 74880 not supported on Linux
  while (!Serial); // Wait until Serial is ready - Leonardo/Micro
}

void loop() {
  TestRunner::run();
}
