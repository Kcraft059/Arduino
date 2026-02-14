#include <master.h>

#define LCD_ADRR 58

SerialInterpreter* srl_inter;
HTI2CLCD lcd;

byte ht_logo[8] = {
    B10100,
    B10100,
    B11111,
    B10110,
    B10110,
    B00010,
    B00010,
};

uint8_t showStr(char** args) {
  lcd.clear(LCD_ADRR);
  lcd.print(LCD_ADRR, args[1]);
  return 0;
}

void setup() {
  // Begin I2C connection
  Wire.begin();

  lcd.setType(LCD_ADRR, 4, 20);
  lcd.createChar(LCD_ADRR, 0, ht_logo);

  lcd.clear(LCD_ADRR);
  lcd.backLight(LCD_ADRR, 100);

  lcd.write(LCD_ADRR, 0);
  lcd.print(LCD_ADRR, " HobbyTronics ");

  // Init a serial connection and interpreter
  Serial.begin(9600);
  serialCmd cmds[] = {{"pr", showStr},
                      {NULL, NULL}};
  srl_inter = new SerialInterpreter(cmds);

#ifdef DEBUG
  Serial.println("[init]: Compiled for debug mode");
#endif
}

void loop() {
  srl_inter->listen();
}
