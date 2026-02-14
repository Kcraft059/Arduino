#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

// Sized Array
template <typename AT, typename ST>
struct sizedArray {
  AT* array;
  ST size;
};

// Struct Schemes
struct digit_pinout {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t f;
  uint8_t g;
  uint8_t DP;
  uint8_t BR;
};

typedef struct {
  char chr;
  uint8_t encoding;
} char_7seg;

// Digit class
class Digit {
public:
  struct digit_pinout pinout = {.a = 6,
                                .b = 7,
                                .c = 12,
                                .d = 11,
                                .e = 10,
                                .f = 8,
                                .g = 9,
                                .DP = 13,
                                .BR = 3};

private:
  sizedArray<char_7seg, uint8_t> encoding;

public:
  Digit();
  ~Digit();

  bool showChar(char);
  void showRaw(uint8_t);
  void setBrightness(uint8_t val);
};

#endif