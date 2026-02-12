#include "include/main.h"

struct srlstr inputstr;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Init");
}

void loop() {
  while (Serial.available()) { // To read continous string at once
    char inchr;

    if ((inputstr.cnt >= inputstr.max - 1) ||                                // End parsing when reaching end of string
        (inchr = Serial.read()) == '\n' || inchr == '\r' || inchr == '\0') { // Or a carriage return
      inputstr.str[inputstr.cnt] = '\0';
      cmdParse(inputstr.str);
      inputstr.cnt = 0;

    } else
      inputstr.str[inputstr.cnt++] = inchr;
  }
}

void cmdParse(char* cmd) {
  Serial.println("Got: \"" + String(cmd) + "\"");
  if (strcmp(cmd, "light") == 0)
    digitalWrite(LED_BUILTIN, HIGH);
  else if (strcmp(cmd, "dark") == 0)
    digitalWrite(LED_BUILTIN, LOW);
}
