#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

uint8_t mask = (1 << PORTB5);

struct __attribute((packed)) mlc_header {
  uint8_t sizel;
  uint8_t offsetl;
  uint8_t sizeh_offseth;
};

extern uint8_t alc_flags;
extern void* malloc(uint16_t);
extern struct mlc_header malloc_get_header(void*);

void blink_bin(uint16_t inp);
void blink_iter(uint16_t itr);
void fast_blink();

void c_entry() {
  uint16_t* addr = malloc(4);
  uint16_t* addr2 = malloc(3);
  
  if (addr != 0) {
    *addr = 1;
    struct mlc_header hdr = malloc_get_header(addr);
    blink_iter(*addr);
    _delay_ms(3000);
    blink_iter((uint16_t)(hdr.sizel | ((hdr.sizeh_offseth & 0xf) << 8)));
    _delay_ms(3000);
    blink_bin((uint16_t)(hdr.offsetl | ((hdr.sizeh_offseth >> 4) << 8)));
  }
  _delay_ms(5000);
  
  if (addr2 != 0) {
    *addr2 = 2;
    struct mlc_header hdr2 = malloc_get_header(addr2);
    blink_iter(*addr2);
    _delay_ms(3000);
    blink_iter((uint16_t)(hdr2.sizel | ((hdr2.sizeh_offseth & 0xf) << 8)));
    _delay_ms(3000);
    blink_bin((uint16_t)(hdr2.offsetl | ((hdr2.sizeh_offseth >> 4) << 8)));
  }
}

void fast_blink() {
  PORTB &= ~mask;
  _delay_ms(100);
  PORTB |= mask;
  _delay_ms(100);
  PORTB &= ~mask;
}

void blink_iter(uint16_t itr) {
  for (; itr > 0; itr--) {
    fast_blink();
  }
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