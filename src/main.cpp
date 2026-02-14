#include <main.h>

SerialInterpreter* srl_inter;
Digit* digit_display;

void setup() {
  Serial.begin(9600);

#ifdef DEBUG
  Serial.println("[init]: debug mode");
#endif

  // Serial command interpreter init
  serialCmd cmds[] = {{"cr", charCmd},
                      {"rw", rawCmd},
                      {"br", brightnessCmd},
                      {"st", stringCmd},
                      {NULL, NULL}};
  srl_inter = new SerialInterpreter(cmds);

  // Digit display setup
  digit_display = new Digit();
}

void loop() {
  srl_inter->listen();
}
