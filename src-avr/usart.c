#include <master.h>

void usart_init(uint16_t baud) {
  // See manual -> Table 20-1
  // Set baud rate
  uint16_t ubrr = (F_CPU / (16ul * baud)) - 1;

  // Write baud rate
  UBRR0H = (ubrr >> 8) & 0b1111;
  UBRR0L = ubrr;

  // Enable RX interrupt + RX and TX
  UCSR0B = /* (1 << RXCIE0) | */ (1 << RXEN0) | (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  // Enable global interrupts
  sei();
}

void usart_send(uint8_t data) {
  // Wait for transmit buffer empty
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer
  UDR0 = data;
}

inline char usart_get(void) {
  // Get and return received data from buffer
  return UDR0;
}

inline uint8_t usart_avail() {
  // Wait for data to be received
  return (UCSR0A & (1 << RXC0));
}