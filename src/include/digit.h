#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

struct digitPinout {
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

extern struct digitPinout digitPin;
extern uint8_t digitEnc[];

// Helpers
void digShow(uint8_t digit, struct digitPinout* digp);

#endif