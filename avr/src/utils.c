#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void usart_send(char chr);
void usart_init(uint16_t ubrr);
void blink_bin(uint16_t inp);
void blink_iter(uint16_t itr);
void fast_blink();

void c_entry() {
  usart_init(9600);
  char* str = malloc(407);
  char* str_ofst = str + 5;
  memcpy(str, "size:", 5);

  struct mlc_header hdr = malloc_get_header(str);
  uint16_t size = hdr.sizel | ((hdr.sizeh_offseth & 0xf) << 8);

  utoa(size, str_ofst, 10);
  for (uint8_t i = 0; str[i] != '\0'; i++)
    usart_send(str[i]);
}

void usart_init(uint16_t baud) {
  // See manual -> Table 20-1
  // Set baud rate
  uint16_t ubrr = (F_CPU / (16ul * baud)) - 1;

  // Write baud rate
  UBRR0H = (ubrr >> 8) & 0b1111;
  UBRR0L = ubrr;
  /*Enable receiver and transmitter */
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  /* Set frame format: 8data, 1stop bit */
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_send(char chr) {
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = chr;
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