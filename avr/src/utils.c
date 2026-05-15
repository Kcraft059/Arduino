#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

uint8_t mask = (1 << PORTB5);

extern void* malloc(uint16_t);

void blink_bin(uint16_t inp);
void fast_blink();

void c_entry() {
  blink_bin((uint16_t)malloc(255));
}

void fast_blink() {
  PORTB &= ~mask;
  _delay_ms(100);
  PORTB |= mask;
  _delay_ms(100);
  PORTB &= ~mask;
}

void blink_bin(uint16_t inp) {
  DDRB = mask;

  for (uint8_t i = 0; i < 16; i++) {
    fast_blink();
    if (inp & (1 << i))
      PORTB |= mask;
    else
      PORTB &= ~mask;
    _delay_ms(500);
  }

  return;
}