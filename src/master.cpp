#include <master.h>

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

SerialInterpreter* srl_inter;
HTlcd* lcd;

void setup() {
  // Begin I2C connection
  Wire.begin();

  // Init a serial connection and interpreter
  Serial.begin(9600);
  serialCmd cmds[] = {{"i2c", sendMsgCmd},
                      {NULL, NULL}};
  srl_inter = new SerialInterpreter(cmds);

  /* lcd = new HTlcd({2, 16});
  lcd->setBrightness(200);

  struct scrollText miamTxt;
  miamTxt.str = "Miam           ";
  miamTxt.delay = 500;
  miamTxt.pos = {1, 1};
  miamTxt.direction = scrollText::LEFT;

  Serial.print("handle" + String(*lcd->addScrollText(miamTxt))); */

#ifdef DEBUG
  Serial.println("[init]: Compiled for debug mode");
#endif
}

void loop() {
  srl_inter->listen();
  lcd->refresh();
}
