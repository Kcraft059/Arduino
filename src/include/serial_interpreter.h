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

struct delimiters {
  char* delims;
  char* beacon;
  char* escape;
};

struct parseAdvancement {
  uint8_t token_start;
  uint8_t token_current;
  byte infos;
};

class SerialInterpreter {
private:
  sizedStr persist_str;
  serialCmd* cmd_list;

public:
  uint16_t max_cmd_length = 64;

public:
  SerialInterpreter(serialCmd*);
  ~SerialInterpreter();

  void listen();                           // Listen to serial port for commands
  char* parse(char*);                      // Parses string from Serial input
  uint8_t eval(char*, struct delimiters*); // Eval a string

private:
  void persistFree();
  static char** tokenizeArgs(char*, struct delimiters*);
  static void stringDelim(char*, struct delimiters*, struct parseAdvancement*);
  static void stringEsc(char*, struct delimiters*, struct parseAdvancement*);
  static void freeArgs(char**);
};

#endif