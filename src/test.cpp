#include "include/header.h"

void myCycle() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(CYCLE_TIME);
  digitalWrite(LED_BUILTIN, LOW);
  delay(CYCLE_TIME);
};