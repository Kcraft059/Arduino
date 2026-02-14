#include <main.h>

uint8_t charCmd(char** args) {
  return !(digit_display->showChar(args[1][0]));
}

uint8_t rawCmd(char** args) {
  char* endptr;
  uint8_t value = strtod(args[1], &endptr);
  if (endptr == args[1]) return 1;

  digit_display->showRaw(value);

  return 0;
}

uint8_t brightnessCmd(char** args) {
  char* endptr;
  uint8_t value = strtod(args[1], &endptr);
  if (endptr == args[1]) return 1;

  digit_display->setBrightness(value);

  return 0;
}

uint8_t stringCmd(char** args) {
  char* endptr;
  int16_t time_delay = strtod(args[2], &endptr);

  if (endptr == args[2]) return 1;

  for (uint8_t i = 0; args[1][i] != '\0'; i++) {
    digit_display->showChar(args[1][i]);
    delay(time_delay);
  }

  return 0;
}