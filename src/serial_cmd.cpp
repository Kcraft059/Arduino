#include <serial_cmd.h>

// Serial parsing
void serialParse(struct persistentStr* pStr, struct serialCmd* cmdList) {
#ifdef CMD_NO_BLOCK
  if (Serial.available()) { // To read one letter at a time
#else
  while (Serial.available()) { // To read continous string at once if contiguous
#endif

    if (pStr->str == NULL)
      pStr->str = (char*)malloc(++(pStr->size) * sizeof(char));
    else
      pStr->str = (char*)realloc(pStr->str, ++(pStr->size) * sizeof(char));

    char chr;

    if (pStr->size >= 256)
      goto reset_str;

    else if ((chr = Serial.read()) == '\n' || chr == '\r' || chr == '\0') { // If not at the end, parse and check for a carriage return
      pStr->str[pStr->size - 1] = '\0';
      cmdEval(pStr->str, cmdList);

    reset_str:
      free(pStr->str);
      pStr->size = 0;
      pStr->str = NULL;

    } else
      pStr->str[pStr->size - 1] = chr;
  }
};

// Command evaluation
void cmdEval(char* cmdStr, struct serialCmd* cmdList) {
  char** args = cmdTokenize(cmdStr, " "); // Split cmdStr by spaces

#ifdef DEBUG
  Serial.println("[key]: \"" + String(args[0]) + "\"");
#endif

  int cmdReturn = -1;
  for (int i = 0; cmdList[i].keyword != NULL; i++)
    if (strcmp(args[0], cmdList[i].keyword) == 0) // Verify if command exists
      cmdReturn = cmdList[i].func(args);          // Execute associated command

#ifdef DEBUG
  Serial.println("[cmd status]: " + String(cmdReturn));
#endif

  argsFree(args); // Free command args
}

// Tokenize commands into args 2d list
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

// Free arg list
void argsFree(char** args) {
  for (int i = 0; args[i] != NULL; i++)
    free(args[i]);
  free(args);
}
