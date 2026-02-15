#include <master.h>

SerialInterpreter* srl_inter;
HTlcd* lcd;

uint8_t sendMsgCmd(char** args) {
  char* endptr;
  byte value = strtod(args[1], &endptr);
  if (endptr == args[1]) return 1;

  Wire.beginTransmission(value);

  for (uint8_t i = 2; args[i]; i++) {

    if (args[i][0] == '&') {
      for (uint8_t j = 1; args[i][j]; j++)
        Wire.write(args[i][j]);

    } else {
      char* endptr;
      value = strtod(args[i], &endptr);
      if (endptr == args[i]) return 1;

      Wire.write((byte)value);
    }
  }

  return Wire.endTransmission();
};

uint8_t scrollCmd(char** args) {
  if (strcmp(args[0], "ascr") == 0) {
    struct scrollText text;
    if (args[1]) text.str = args[1];
    else return 1;
    if (args[2]) text.dir = (args[2][0] == 'r') ? scrollText::RIGHT : scrollText::LEFT;
    else return 1;
    if (args[3]) text.dly = strtod(args[3], NULL);
    else return 1;
    if (args[4]) text.pos.line = strtod(args[4], NULL);
    else return 1;
    if (args[5]) text.pos.column = strtod(args[5], NULL);
    else return 1;
    if (args[6]) text.len = strtod(args[6], NULL);
    else return 1;

    char strbuffer[32];
    sprintf(strbuffer, "[lcd]: %p", lcd->addScrollText(text));

    Serial.println(strbuffer);
    return 0;
  } else {
    uint8_t* handle;
    if (args[1]) handle = (uint8_t*)strtoul(args[1], NULL, 0);
    else return 1;

    char strbuffer[32];
    sprintf(strbuffer, "[lcd]: %p", handle);

    Serial.println(strbuffer);

    return lcd->delScrollText(*handle);
  }
};

void setup() {
  // Begin I2C connection
  Wire.begin();

  // Init a serial connection and interpreter
  Serial.begin(9600);
  serialCmd cmds[] = {{"i2c", sendMsgCmd},
                      {"ascr", scrollCmd},
                      {"rscr", scrollCmd},
                      {NULL, NULL}};
  srl_inter = new SerialInterpreter(cmds);

  lcd = new HTlcd({2, 16});

  lcd->setBrightness(100);
  lcd->clearAll();

#ifdef DEBUG
  Serial.println("[init]: Compiled for debug mode");
#endif
}

void loop() {
  srl_inter->listen();
  lcd->refresh();
}
