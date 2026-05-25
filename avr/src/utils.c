#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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
void usart_send_str(char* str);

void c_entry() {
  usart_init(9600);
  char str[24];

  char* str1 = malloc(64);
  utoa((uint16_t)str1, str, 16);
  usart_send_str(str);

  char* str2 = malloc(64);
  utoa((uint16_t)str2, str, 16);
  usart_send_str(str);

  char* str3 = malloc(10);
  utoa((uint16_t)str3, str, 16);
  usart_send_str(str);

  free(str1);
	free(str2);

  char* str4 = malloc(127);
  utoa((uint16_t)str4, str, 16);
  usart_send_str(str);

  char* str5 = malloc(2);
  utoa((uint16_t)str5, str, 16);
  usart_send_str(str);

  utoa(malloc_get_header(str5).offsetl, str, 10);
  usart_send_str(str);
  utoa(malloc_get_header(str5).sizel, str, 10);
  usart_send_str(str);
}

inline void usart_send_str(char* str) {
  for (uint8_t i = 0; str[i] != '\0'; i++)
    usart_send(str[i]);
  usart_send('\n');
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
