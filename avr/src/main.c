#include <avr/io.h>

uint8_t mask = (1 << PORTB5);

void ledOn() {
  DDRB = PORTB = mask;
  return;
}