#include <serial_interpreter.h>

SerialInterpreter::SerialInterpreter(serialCmd* cmd_list) {
  uint8_t cmd_ls_size = 0;
  while (cmd_list[cmd_ls_size++].keyword != NULL)
    cmd_list[cmd_ls_size - 1].keyword = strdup(cmd_list[cmd_ls_size - 1].keyword);

  this->cmd_list = (serialCmd*)malloc(cmd_ls_size * sizeof(serialCmd));
  memcpy(this->cmd_list, cmd_list, cmd_ls_size * sizeof(serialCmd));

  this->persist_str.str = NULL;
  this->persist_str.count = 0;
  this->persist_str.size = 0;
};

SerialInterpreter::~SerialInterpreter() {
  for (uint8_t i = 0; this->cmd_list[i].keyword != NULL; i++)
    free(this->cmd_list[i].keyword);
  free(this->cmd_list);

  if (this->persist_str.str)
    free(this->persist_str.str);
}

void SerialInterpreter::listen() {
  char* srl_cmd = this->parse("\n\r");

  if (srl_cmd) {
    struct delimiters delims{
        " ",
        "\"\'",
        "\\"};

    uint8_t status = this->eval(srl_cmd, &delims);
    free(srl_cmd);
  }
}

char* SerialInterpreter::parse(char* delims) {
  char* parsed_str = NULL;

  while (Serial.available()) { // To read continous string at once if contiguous
    if (++(this->persist_str.count) > this->max_cmd_length) {
      this->persistFree();
      return parsed_str;
    }

    if (!this->persist_str.str)
      this->persist_str.str = (char*)malloc((this->persist_str.size += PARSE_ALLOC_INCREMENT) * sizeof(char));
    else if (this->persist_str.size < this->persist_str.count)
      this->persist_str.str = (char*)realloc(this->persist_str.str,
                                             (this->persist_str.size += PARSE_ALLOC_INCREMENT) * sizeof(char));

    char chr = Serial.read();
    bool is_delim = false;

    for (uint8_t i = 0; delims[i] != '\0' && !is_delim; i++)
      if (chr == delims[i])
        is_delim = true;

    if (is_delim) {
      this->persist_str.str[this->persist_str.count - 1] = '\0';

      parsed_str = strdup(this->persist_str.str);
      this->persistFree();

#ifdef DEBUG
      Serial.println("[cmd]: \"" + String(parsed_str) + "\"");
#endif

      return parsed_str;

    } else
      this->persist_str.str[this->persist_str.count - 1] = chr;
  }

  return parsed_str;
};

uint8_t SerialInterpreter::eval(char* cmd_str, struct delimiters* delims) {
  char** args = this->tokenizeArgs(cmd_str, delims);

#ifdef DEBUG
  Serial.println("[cmd][key]: \"" + String(args[0]) + "\"");
#endif

  uint8_t cmd_status = ~0;
  for (int i = 0; this->cmd_list[i].keyword != NULL; i++)
    if (strcmp(args[0], this->cmd_list[i].keyword) == 0) // Verify if command exists
      cmd_status = this->cmd_list[i].func(args);         // Execute associated command

  freeArgs(args); // Free command args

#ifdef DEBUG
  Serial.println("[cmd][sts]: " + String(cmd_status));
#endif

  return cmd_status;
};

char** SerialInterpreter::tokenizeArgs(char* str, struct delimiters* delims) {
#ifdef DEBUG
  Serial.print("[cmd][tok]: ");
#endif

  int arglen = 1;
  char** args = (char**)malloc(arglen * sizeof(char*)); // Allocs an argument array

  struct parseAdvancement p = {};

  while (1) {
    /// Post check

    SerialInterpreter::stringDelim(str, delims, &p);

    // If delim has been reached
    if (p.infos & (1 << 0)) {

      if (p.token_current - p.token_start > 0) { // If arg is non-null string, store it in args[]
        // Copy arg
        args[arglen - 1] = (char*)malloc((p.token_current - p.token_start + 1) * sizeof(char)); // Allocs a new string in args
        memcpy(args[arglen - 1], str + p.token_start, p.token_current - p.token_start);         // Copy memory
        args[arglen - 1][p.token_current - p.token_start] = '\0';                               // End arg with null to make a cstr

        // Expand args
        args = (char**)realloc(args, ++arglen * sizeof(char*)); // Expand args

#ifdef DEBUG
        Serial.print("|\"" + String(args[arglen - 2]) + "\"");
#endif
      }

      if (str[p.token_current] == '\0')
        break; // Break if we reached the end of the string

      // Update infos
      p.token_start = ++(p.token_current); // Collapse token range and go to next char
      p.infos &= ~(1 << 0);                // Resets delim info

    } else
      // Delim has not been reached
      p.token_current++; // Go to next char
  }

#ifdef DEBUG
  Serial.print("|[\\0]\n");
#endif

  args[arglen - 1] = NULL; // Terminate arg list with NULL
  return args;
};

void SerialInterpreter::stringDelim(char* str, struct delimiters* delims, struct parseAdvancement* p) {
  /// Checks

  // Check for end of string
  if (str[p->token_current] == '\0') {
    p->infos |= (1 << 0);
    return;
  }

  // If is not escaped
  if (p->infos & (1 << 1)) {
    SerialInterpreter::stringEsc(str, delims, p);
    return;
  }

  // Check for delims
  if (!(p->infos >> 2)) // If no beacon
    for (uint8_t i = 0; delims->delims[i]; i++)
      if (str[p->token_current] == delims->delims[i]) {
        p->infos |= (1 << 0);

        SerialInterpreter::stringEsc(str, delims, p);
        return;
      }

  // Check for beacons
  for (uint8_t i = 0; delims->beacon[i]; i++)
    if (str[p->token_current] == delims->beacon[i])                     // If current char is a beacon delim
      if (p->token_current - p->token_start == 0 && !(p->infos >> 2)) { // If we are at start of args + there's no previous beacon
        p->infos |= (i + 1) << 2;                                       // Set last beacon id to i
        p->token_start = p->token_current + 1;                          // Move token start pointer after beacon

        SerialInterpreter::stringEsc(str, delims, p);
        return;

      } else if ((p->infos >> 2) == i + 1) { // If previous beacon is same char, end arg
        p->infos &= ~(~(0) << 2);            // Reset last beacon id to 0
        p->infos |= (1 << 0);                // Set delim info to 1

        SerialInterpreter::stringEsc(str, delims, p);
        return;
      }

  SerialInterpreter::stringEsc(str, delims, p);
}

void SerialInterpreter::stringEsc(char* str, struct delimiters* delims, struct parseAdvancement* p) {
  // Check for esc char
  for (uint8_t i = 0; delims->escape[i]; i++)
    if (str[p->token_current] == delims->escape[i] && !(p->infos & (1 << 1))) { // If current char is '\' and is not escaped
      p->infos |= (1 << 1);                                                     // Set escape status
      memmove(str + p->token_current,                                           // Move string to the left one char to remove esc char
              str + p->token_current + 1,
              strlen(str + p->token_current));
      p->token_current--; // Since string has been moved to current char, we need to move back the current token pointer
      return;
    }

  p->infos &= ~(1 << 1); // Remove escape status
}

void SerialInterpreter::freeArgs(char** args) {
  for (int i = 0; args[i] != NULL; i++)
    free(args[i]);
  free(args);
};

void SerialInterpreter::persistFree() {
  free(this->persist_str.str);
  this->persist_str.str = NULL;
  this->persist_str.count = 0;
  this->persist_str.size = 0;
}