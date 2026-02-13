#include "WString.h"
#include "include/main.h"

/// Serial command defs

/// Top level eval and parsing
// Serial parsing
void serialParse(struct persistentStr* pStr, struct serialCmd* cmdList) {

#ifdef CMD_NO_BLOCK
  if (Serial.available()) { // To read one letter at a time
#else
  while (Serial.available()) { // To read continous string at once if contiguous
#endif

    char chr;

    if ((pStr->cnt >= pStr->max - 1) ||                                // End when reaching end of string
        (chr = Serial.read()) == '\n' || chr == '\r' || chr == '\0') { // If not at the end, parse and check for a carriage return

      pStr->str[pStr->cnt] = '\0';
      pStr->cnt = 0;

      cmdEval(pStr->str, cmdList);

    } else
      pStr->str[pStr->cnt++] = chr;
  }
};

void cmdEval(char* cmdStr, struct serialCmd* cmdList) {
  char** args = cmdTokenize(cmdStr, " ");

#ifdef DEBUG
  Serial.println("[key]: \"" + String(args[0]) + "\"");
#endif

  int cmdReturn = -1;
  for (int i = 0; cmdList[i].keyword != NULL; i++) {
    if (strcmp(args[0], cmdList[i].keyword) == 0)
      cmdReturn = cmdList[i].func(args);
  }

#ifdef DEBUG
  Serial.println("[cmd status]: " + String(cmdReturn));
#endif

  argsFree(args); // Free command args
}

char** cmdTokenize(char* str, char* separator) {
#ifdef DEBUG
  Serial.print("[tok]: \"");
#endif

  // Copy original string
  const size_t cmdlen = sizeof(char*) * (strlen(str) + 1);
  char* strBuffer = (char*)malloc(cmdlen);
  if (!memcpy(strBuffer, str, cmdlen))
    return NULL;

  int arglen = 1;
  char** args = (char**)malloc(arglen * sizeof(char*)); // Allocs an argument array

  // Tokenize str
  char* token = strtok(strBuffer, separator);

  while (token != NULL) {
    char** currentArg = args + arglen - 1;
    size_t toklength = (strlen(token) + 1) * sizeof(char);

    // Copy token value
    *currentArg = (char*)malloc(toklength);
    if (!memcpy(*currentArg, token, toklength))
      return NULL;

#ifdef DEBUG
    Serial.print("|" + String(*currentArg));
#endif

    // Expend args
    args = (char**)realloc(args, ++arglen * sizeof(char*));
    token = strtok(NULL, " "); // Get to next token
  }

#ifdef DEBUG
  Serial.print("\"\n");
#endif

  args[arglen - 1] = NULL;

  free(strBuffer);
  return args;
}

void argsFree(char** args) {
  for (int i = 0; args[i] != NULL; i++)
    free(args[i]);
  free(args);
}
