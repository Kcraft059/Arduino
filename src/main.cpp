#include "include/main.h"

struct srlstr inputstr;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Init");
}

void loop() {
  char inchr = Serial.read();

  if (inchr != -1) {
    inputstr.cnt = 0;
    while (inputstr.cnt < inputstr.max - 1) {
      if (inchr != -1) {
        if (inchr == '\n' || inchr == '\r')
          break;
        else
          inputstr.str[inputstr.cnt] = inchr;

        inputstr.cnt++;
      }
      inchr = Serial.read();
    }

    inputstr.str[inputstr.cnt] = '\0';

    Serial.println("Got: " + String(inputstr.str));

    if (strcmp(inputstr.str, "light") == 0)
      digitalWrite(LED_BUILTIN, HIGH);
    if (strcmp(inputstr.str, "dark") == 0)
      digitalWrite(LED_BUILTIN, LOW);
  }

  delay(500);
}