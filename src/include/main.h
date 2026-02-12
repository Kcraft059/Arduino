#include <Arduino.h>

#define SERIAL_BUFFER 256

struct srlstr {
  char str[SERIAL_BUFFER];
  int cnt = 0;
  int max = SERIAL_BUFFER;
};

void cmdParse(char*);