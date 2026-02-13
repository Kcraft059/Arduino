#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H

#include <Arduino.h>

struct persistentStr {
  char* str;
  size_t size;
};

struct serialCmd {
  char* keyword;
  int (*func)(char**);
};

// Serial eval
void serialParse(struct persistentStr*, struct serialCmd*);
void cmdEval(char*, struct serialCmd*);

char** cmdTokenize(char*, char*);
void argsFree(char**);

#endif