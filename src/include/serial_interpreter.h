#ifndef SERIAL_INTERPRETER_H
#define SERIAL_INTERPRETER_H

#include <Arduino.h>

typedef struct {
  char* str;
  int size;
} sizedStr;

typedef struct {
  char* keyword;
  uint8_t (*func)(char**);
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

  void listen();              // Listen to serial port for commands
  char* parse(char*);         // Parses string from Serial input
  uint8_t eval(char*, char*); // Eval a string

private:
  void persistFree();
  static char** tokenizeArgs(char*, char*);
  static void freeArgs(char**);
};

#endif