#include <serial_interpreter.h>

SerialInterpreter::SerialInterpreter(serialCmd* cmd_list) {
  uint8_t cmd_ls_size = 0;
  while (cmd_list[cmd_ls_size++].keyword != NULL)
    cmd_list[cmd_ls_size - 1].keyword = strdup(cmd_list[cmd_ls_size - 1].keyword);

  this->cmd_list = (serialCmd*)malloc(cmd_ls_size * sizeof(serialCmd));
  memcpy(this->cmd_list, cmd_list, cmd_ls_size * sizeof(serialCmd));

  this->persist_str.str = NULL;
  this->persist_str.size = 0;
};

SerialInterpreter::~SerialInterpreter() {
  for (uint8_t i = 0; this->cmd_list[i].keyword != NULL; i++)
    free(cmd_list[i].keyword);
  free(this->cmd_list);

  if (this->persist_str.str)
    free(this->persist_str.str);
}

void SerialInterpreter::listen() {
  char* srl_cmd = this->parse(";\n\r");
  
  if (srl_cmd) {
    uint8_t status = this->eval(srl_cmd, " ");
    free(srl_cmd);
  }
}

char* SerialInterpreter::parse(char* delims) {
  char* parsed_str = NULL;

  while (Serial.available()) { // To read continous string at once if contiguous
    if (++(this->persist_str.size) > this->max_cmd_length) {
      this->persistFree();
      return parsed_str;
    }

    if (!this->persist_str.str)
      this->persist_str.str = (char*)malloc(this->persist_str.size * sizeof(char));
    else
      this->persist_str.str = (char*)realloc(this->persist_str.str, this->persist_str.size * sizeof(char));

    char chr = Serial.read();
    bool is_delim = false;

    for (uint8_t i = 0; delims[i] != '\0'; i++)
      if (chr == delims[i]) {
        is_delim = true;
        break;
      };

    if (is_delim) {
      this->persist_str.str[this->persist_str.size - 1] = '\0';

      parsed_str = strdup(this->persist_str.str);
      this->persistFree();

#ifdef DEBUG
      Serial.println("[cmd]: \"" + String(parsed_str) + "\"");
#endif

      return parsed_str;

    } else
      this->persist_str.str[this->persist_str.size - 1] = chr;
  }

  return parsed_str;
};

uint8_t SerialInterpreter::eval(char* cmd_str, char* delims) {
  char** args = this->tokenizeArgs(cmd_str, delims);

#ifdef DEBUG
  Serial.println("[cmd][key]: \"" + String(args[0]) + "\"");
#endif

  uint8_t cmd_status = -1;
  for (int i = 0; this->cmd_list[i].keyword != NULL; i++)
    if (strcmp(args[0], this->cmd_list[i].keyword) == 0) // Verify if command exists
      cmd_status = this->cmd_list[i].func(args);         // Execute associated command

  freeArgs(args); // Free command args

#ifdef DEBUG
  Serial.println("[cmd][sts]: " + String(cmd_status));
#endif

  return cmd_status;
};

char** SerialInterpreter::tokenizeArgs(char* cmd_str, char* delims) {
#ifdef DEBUG
  Serial.print("[cmd][tok]: ");
#endif

  int arglen = 1;
  char** args = (char**)malloc(arglen * sizeof(char*)); // Allocs an argument array

  // Tokenize str
  char* token = strtok(cmd_str, delims);

  while (token != NULL) {
    // Copy token value
    args[arglen - 1] = strdup(token);

#ifdef DEBUG
    Serial.print("|" + String(token));
#endif

    // Expend args
    args = (char**)realloc(args, ++arglen * sizeof(char*));
    token = strtok(NULL, delims); // Get to next token
  }

#ifdef DEBUG
  Serial.print("|\n");
#endif

  args[arglen - 1] = NULL;
  return args;
};

void SerialInterpreter::freeArgs(char** args) {
  for (int i = 0; args[i] != NULL; i++)
    free(args[i]);
  free(args);
};

void SerialInterpreter::persistFree() {
  free(this->persist_str.str);
  this->persist_str.str = NULL;
  this->persist_str.size = 0;
}