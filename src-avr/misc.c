#include <lwutils.h>

/// MARK: USART

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

// ISR(USART_RX_vect) {
//   PORTB ^= (1 << PB5); // Turn on led
//   char c = usart_get();
// }

/// MARK: EEPROM

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

/// MARK: Int0

// ISR(INT0_vect) {
//   PORTB |= (1 << PB5); // Turns portb5 to high;
// };

void init_int0() {
  DDRD &= ~(1 << PD2);  // Set PD2 as input
  PORTD &= ~(1 << PD2); // pull-up

  EICRA |= ((1 << ISC01) | (1 << ISC00)); // Set rising edge as int0 trigger

  EIMSK |= (1 << INT0); // Allow int0 trigger

  sei(); // Enable all interrupts
}

/// MARK: Timer0/Clock0

// volatile uint8_t timer0_millis = 0;

// ISR(TIMER0_OVF_vect) {
//   timer0_millis++;
// }

void timer0_init(uint8_t clks) {
  TCCR0A = 0;                                            // normal mode
  TCCR0B = (TCCR0B & ~0b111) | ((clks & 0b111) << CS00); // prescaler to prescale
  TIMSK0 = (1 << TOIE0);                                 // enable overflow interrupt
  sei();                                                 // enable global interrupts
}

/// MARK: PWM with Clock0

inline void init_CL0(uint8_t mode) {
  TCCR0A = (0b10 << COM0A0) | (0b11 << WGM00); // Non inverting mode + Configure in fast PWM
  TCCR0B = (mode & 0b111);                     // Only take mode bits from mode
};

inline void set_cmp_OC0A(uint8_t cmp) {
  OCR0A = cmp; // Set compare register to threshold
}

/// MARK: ADC

inline void init_ADC(uint8_t prescaler) {
  ADCSRA = (1 << ADEN) | ((prescaler & 0b111) << ADPS0);
};

uint16_t read_analog_ADC(uint8_t pinNum) {
  ADMUX = (1 << REFS0) | ((pinNum & 0b1111) << MUX0); // REFS0, AVCC with external capacitor at AREF pin, MUX0 0b0000 input as ADC0
  ADCSRA |= (1 << ADSC);                              // Start conversion

  while (ADCSRA & (1 << ADSC))
    ; // Wait for end of conversion

  return ADCL | ((uint16_t)ADCH << 8);
};
