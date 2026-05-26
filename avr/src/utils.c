#define ALC_TABLE_NUM 16

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include <alloc.h>


uint8_t mask = (1 << PORTB5);

void usart_send(char chr);
void usart_init(uint16_t ubrr);
void blink_bin(uint16_t inp);
void blink_iter(uint16_t itr);
void fast_blink();
void usart_send_str(char* str);
uint8_t validate_heap(void* start);

void c_entry() {
  usart_init(9600);

  char buf[32];
  uint8_t* allc = malloc(0);

  free(allc);
  validate_heap(allc);

  void* allocs[ALC_TABLE_NUM] = {};

  for (uint16_t i = 0; i < 100; i++) {
    uint8_t idx = rand() % ALC_TABLE_NUM;

    if (allocs[idx] == NULL) {
      uint16_t sz = rand() % 400;

      sprintf(buf, "-> malloc(%d)", sz);
      usart_send_str(buf);

      if ((allocs[idx] = malloc(sz)) == NULL)
        usart_send_str("[null]");

    } else {
      sprintf(buf, "-> free(%p)", allocs[idx]);
      usart_send_str(buf);

      free(allocs[idx]);
      allocs[idx] = NULL;
    }
    if (validate_heap(allc))
      return;
  }
}

uint8_t validate_heap(void* ptr) {
  struct alc_header hdr = mlchdr(ptr);
  int size = hdr.sizel | ((hdr.sizeh_offseth & 0xf) << 8);
  int offset = hdr.offsetl | ((hdr.sizeh_offseth >> 4) << 8);

  char str[32];

  while (1) {
    sprintf(str, "[p:%p][s:%d][o:%d]", ptr, size, offset);
    for (int i = 0; str[i] != 0; i++)
      usart_send(str[i]);

    ptr = (uint8_t*)(size + offset + 3 + (uint16_t)ptr);

    if (offset == 0xfff) break;

    if ((uint16_t)ptr >= RAMEND) {
      usart_send_str("[iv]");
      return 1;
    }
    hdr = mlchdr(ptr);
    size = hdr.sizel | ((hdr.sizeh_offseth & 0xf) << 8);
    offset = hdr.offsetl | ((hdr.sizeh_offseth >> 4) << 8);
  }
  usart_send_str("[v]");
  return 0;
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
