#ifndef SERIAL_INTERPRETER_H
#define SERIAL_INTERPRETER_H

#include <Arduino.h>

typedef struct {
  char* str;
  int size;
} sizedStr;

typedef struct {
  char* keyword;
  int (*func)(char**);
} serialCmd;

class SerialInterpreter {
private:
  sizedStr persist_str;
  serialCmd* cmd_list;

public:
  uint16_t max_cmd_length = 64;

public:
  SerialInterpreter(serialCmd*);
  ~SerialInterpreter();

  char* parse(char*); // Parses command from Serial input
  uint8_t eval(char*);

  static char** tokenize(char*, char*);
  static void argsFree(char**);
};

#endif