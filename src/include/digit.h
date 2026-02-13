#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

struct digitPinout {
  int a;
  int b;
  int c;
  int d;
  int e;
  int f;
  int g;
  int DP;
  int BR;
};

extern struct digitPinout digitPin;
extern int8_t digitEnc[];

// Helpers
void digShow(int digit, struct digitPinout* digp);

#endif