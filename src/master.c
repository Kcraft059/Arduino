#define F_CPU 16000000UL // 16000 Mghz -> 16 * 10^3 hz

#include <master.h>

void uart_init(int baud) {
  // See manual -> Table 20-1
  // Set baud rate
  uint16_t ubrr = (F_CPU / (16ul * baud)) - 1;

  // Write baud rate
  UBRR0H = (ubrr >> 8);
  UBRR0L = ubrr;

  // Enable RX interrupt + RX and TX
  UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  // Enable global interrupts
  sei();
}

void uart_send(uint8_t data) {
  // Wait for transmit buffer empty
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Put data into buffer
  UDR0 = data;
}

inline char usart_get(void) {
  /* Get and return received data from buffer */
  return UDR0;
}

inline int usart_avail() {
  /* Wait for data to be received */
  return (UCSR0A & (1 << RXC0));
}

inline void _usart_send_str(char* str) {
  for (int i = 0; str[i]; i++)
    uart_send(str[i]);
}

ISR(USART_RX_vect) {
  PORTB ^= (1 << PB5); // Turn on led
  char c = usart_get();
}

volatile unsigned long timer0_millis = 0;

ISR(TIMER0_OVF_vect) {
  timer0_millis++;
}

void timer0_init(uint8_t prescale) {
  TCCR0A = 0;                                      // normal mode
  TCCR0B = (TCCR0B & ~0b111) | (prescale & 0b111); // prescaler to prescale
  TIMSK0 = (1 << TOIE0);                           // enable overflow interrupt
  sei();                                           // enable global interrupts
}

int main(void) {
  uart_init(9600);
  // timer0_init(3);

  DDRB |= (1 << PB3 | 1 << PB5); // set pins as output

  while (1) {
    PORTB ^= (1 << PB3); // toggle
    _delay_ms(500);
  }
}