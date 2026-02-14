#include <master.h>

#define LCD_ADRR 58

SerialInterpreter* srl_inter;

uint8_t sendMsgCmd(char** args) {
  char* endptr;
  byte value = strtod(args[1], &endptr);
  if (endptr == args[1]) return 1;

  Wire.beginTransmission(value);

  for (uint8_t i = 2; args[i]; i++) {
    byte value;

    if (args[i][0] == '&') {
      value = (byte)args[i][1];
    } else {
      char* endptr;
      value = strtod(args[i], &endptr);
      if (endptr == args[i]) return 1;
    }

    Wire.write((byte)value);
  }

  return Wire.endTransmission();
};

void setup() {
  // Begin I2C connection
  Wire.begin();

  // Init a serial connection and interpreter
  Serial.begin(9600);
  serialCmd cmds[] = {{"i2c", sendMsgCmd},
                      {NULL, NULL}};
  srl_inter = new SerialInterpreter(cmds);

#ifdef DEBUG
  Serial.println("[init]: Compiled for debug mode");
#endif
}

void loop() {
  srl_inter->listen();
}
