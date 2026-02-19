#define F_CPU 16000000UL // 16000 Mghz -> 16 * 10^3 hz

#include <avr/interrupt.h>
#include <avr/io.h>
// #include <stdio.h>
#include <util/delay.h>

void usart_init(int baud) {
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

void usart_send(uint8_t data) {
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

void _usart_send_str(char* str) {
  for (int i = 0; str[i]; i++)
    usart_send(str[i]);
}

uint8_t eeprom_read_byte(const uint16_t addr) {
  while (EECR & (1 << EEPE)) // Wait for previous write completion
    ;

  EEARH = (addr >> 8); // & 0b1; Mask not needed since, you can't write to any other reg
  EEARL = addr & 0xFF;
  // Could use 16-bit reg alias `EEAR = addr;`

  EECR |= (1 << EERE); // Enable read operation

  return EEDR; // Return value from data read operation
}

void eeprom_write_byte(const uint16_t addr, const uint8_t data) {
  while (EECR & (1 << EEPE)) // Wait for previous write completion
    ;

  EEARH = (addr >> 8); // & 0b1; Mask not needed since, you can't write to any other reg
  EEARL = addr & 0xFF;
  // Could use 16-bit reg alias `EEAR = addr;`

  EEDR = data; // Set data

  EECR |= (1 << EEMPE); // Allow data write
  EECR |= (1 << EEPE);  // Start data write
}

void eeprom_erase_byte(const uint16_t addr) {
  while (EECR & (1 << EEPE)) // Wait for previous write completion
    ;

  uint8_t prev_EECR = EECR;

  EECR = (EECR & ~((1 << EEPM1) | (1 << EEPM0))) | 1 << EEPM0; // Set mode to erase only, erase set eeprom reg to 255

  EEARH = (addr >> 8); // & 0b1; Mask not needed since, you can't write to any other reg
  EEARL = addr & 0xFF;
  // Could use 16-bit reg alias `EEAR = addr;`

  EECR |= (1 << EEMPE); // Allow data write
  EECR |= (1 << EEPE);  // Start data write

  EECR = prev_EECR;
}

ISR(INT0_vect) {
  PORTB |= (1 << PB5); // Turns portb5 to high;
};

int main(void) {
  DDRB |= (1 << PB5);   // Set PB5 as output
  DDRD &= ~(1 << PD2);  // Set PD2 as input
  PORTD &= ~(1 << PD2); // pull-up

  EICRA |= ((1 << ISC01) | (1 << ISC00)); // Set rising edge as int0 trigger

  EIMSK |= (1 << INT0); // Allow int0 trigger

  sei(); // Enable all interrupts

  while (1); // Prevent program end
};