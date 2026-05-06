#include <avr/interrupt.h>
#include <avr/io.h>

ISR(INT0_vect) {
  PORTB ^= (1 << PORTB5);
}

void main() {
  DDRB = (1 << DDB5);
  EICRA = (0b11 << ISC00);
  EIMSK = (1 << INT0);
  sei();

  while (1)
    ;
}