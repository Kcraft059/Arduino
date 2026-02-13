#ifndef SERIAL_CMD_H
#define SERIAL_CMD_H

#define SERIAL_BUFFER 256

struct persistentStr {
  char str[SERIAL_BUFFER];
  int cnt = 0;
  int max = SERIAL_BUFFER;
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