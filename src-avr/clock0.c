#include <master.h>

volatile unsigned long timer0_millis = 0;

ISR(TIMER0_OVF_vect) {
  timer0_millis++;
}

void timer0_init(uint8_t prescaler) {
  TCCR0A = 0;                                            // normal mode
  TCCR0B = (TCCR0B & ~0b111) | ((prescaler & 0b111) << CS00); // prescaler to prescale
  TIMSK0 = (1 << TOIE0);                                 // enable overflow interrupt
  sei();                                                 // enable global interrupts
}