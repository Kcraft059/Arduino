#include <digit.h>

uint8_t digitEnc[] = {
    0b00111111, // 0 // Encode number in bin as DP,G,F,E,D,C,B,A
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

struct digitPinout digitPin = {
    .a = 6,
    .b = 7,
    .c = 12,
    .d = 11,
    .e = 10,
    .f = 8,
    .g = 9,
    .DP = 13,
    .BR = 3};

void digShow(uint8_t digit, struct digitPinout* digp) {
  digitalWrite(digp->a, digit & 1 << 0);
  digitalWrite(digp->b, digit & 1 << 1);
  digitalWrite(digp->c, digit & 1 << 2);
  digitalWrite(digp->d, digit & 1 << 3);
  digitalWrite(digp->e, digit & 1 << 4);
  digitalWrite(digp->f, digit & 1 << 5);
  digitalWrite(digp->g, digit & 1 << 6);
  digitalWrite(digp->DP, digit & 1 << 7);
};
