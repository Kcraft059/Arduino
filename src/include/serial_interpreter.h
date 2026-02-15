#ifndef SERIAL_INTERPRETER_H
#define SERIAL_INTERPRETER_H

#include <Arduino.h>

#define PARSE_ALLOC_INCREMENT 8

typedef struct {
  char* str;
  uint8_t count;
  uint8_t size;
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

  void listen();                                      // Listen to serial port for commands
  char* parse(char* delims);                          // Parses string from Serial input
  uint8_t eval(char* str, struct delimiters* delims); // Eval a string

private:
  void persistFree();
  static char** tokenizeArgs(char* str, struct delimiters* delims);
  static void stringDelim(char* str, struct delimiters* delims, struct parseAdvancement* p);
  static void stringEsc(char* str, struct delimiters* delims, struct parseAdvancement* p);
  static void freeArgs(char** args);
};

#endif