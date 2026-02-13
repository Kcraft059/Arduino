#include "include/main.h"

void setup() {
  pinMode(digitPin.a, OUTPUT);
  pinMode(digitPin.b, OUTPUT);
  pinMode(digitPin.c, OUTPUT);
  pinMode(digitPin.d, OUTPUT);
  pinMode(digitPin.e, OUTPUT);
  pinMode(digitPin.f, OUTPUT);
  pinMode(digitPin.g, OUTPUT);
  pinMode(digitPin.DP, OUTPUT);
  pinMode(digitPin.BR, OUTPUT);
  Serial.begin(9600);

#ifdef DEBUG
  Serial.println("[init]");
#endif
}

int numCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  digShow(digitEnc[num], &digitPin);
  return 0;
}

int brCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  analogWrite(digitPin.BR, num);
  return 0;
}

int rawSegCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  digShow(num, &digitPin);
  return 0;
}

struct persistentStr inputstr;
struct serialCmd cmdList[] = {
    {"num", numCmd},
    {"br", brCmd},
    {"raw", rawSegCmd},
    {NULL}};

void loop() {
  serialParse(&inputstr, cmdList); // Interprets serial commands
}
