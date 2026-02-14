#include <main.h>

unsigned long timeMark = 0;
unsigned long timeDelay = 50;
bool counter = false;
int8_t count = 0;

int numCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  digShow(digitEnc[num], &digitPin);
  return 0;
}

int brCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  analogWrite(digitPin.BR, num);
  return 0;
}

int rawSegCmd(char** args) {
  char* endptr;
  int num = strtod(args[1], &endptr);

  if (args[1] == endptr)
    return 1;

  digShow(num, &digitPin);
  return 0;
}

int timeCmd(char** args) {

  if (strcmp(args[1], "dl") == 0) {
    char* endptr;
    unsigned long num = strtod(args[2], &endptr);

    if (args[2] == endptr)
      return 1;

    timeDelay = num;
    timeMark = millis();

  } else if (strcmp(args[1], "st") == 0) {

    switch (args[2][0]) {
    case 't':
      counter ^= 1;
      break;
    case '1':
      counter = 1;
      break;
    case '0':
      counter = 0;
      break;
    default:
      return 1;
    }
  } else {
    return 1;
  }
  return 0;
}

SerialInterpreter* srl_inter;

void setup() {
  pinMode(digitPin.a, OUTPUT);
  pinMode(digitPin.b, OUTPUT);
  pinMode(digitPin.c, OUTPUT);
  pinMode(digitPin.d, OUTPUT);
  pinMode(digitPin.e, OUTPUT);
  pinMode(digitPin.f, OUTPUT);
  pinMode(digitPin.g, OUTPUT);
  pinMode(digitPin.DP, OUTPUT);
  pinMode(digitPin.BR, OUTPUT);
  Serial.begin(9600);

#ifdef DEBUG
  Serial.println("[init]: debug mode");
#endif

  serialCmd cmds[] = {{"num", numCmd},
                      {"br", brCmd},
                      {"raw", rawSegCmd},
                      {"tm", timeCmd},
                      {NULL, NULL}};

  srl_inter = new SerialInterpreter(cmds);
}

void loop() {
  char* srl_cmd;
  if (srl_cmd = srl_inter->parse("\n\r")) {
    uint8_t status = srl_inter->eval(srl_cmd);
    free(srl_cmd);
  }

  if ((millis() >= timeMark) && counter) {
    timeMark = millis() + timeDelay;

    digShow(count, &digitPin);
    count++;
  }
}
