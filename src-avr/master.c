#include <master.h>

#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

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

inline void init_CL0(uint8_t mode) {
  TCCR0A = (0b10 << COM0A0) | (0b11 << WGM00); // Non inverting mode + Configure in fast PWM
  TCCR0B = (mode & 0b111);                     // Only take mode bits from mode
};

inline void set_cmp_OC0A(uint8_t cmp) {
  OCR0A = cmp; // Set compare register to threshold
}

int main(void) {
  ADCSRA = (1 << ADEN) | ((5 & 0b111) << ADPS0);                // Reduce precision, because only 8 bit first bits needed
  ADMUX = (1 << REFS0) | (1 << ADLAR) | ((0 & 0b1111) << MUX0); // Ref set at Vcc, ADLAR (left align), pin 0
  DDRC &= ~(1 << PC0);                                          // Set PC0 as input
  DIDR0 |= (1 << ADC0D);                                        // Disable digital on pin ADC0

  TCCR0A = (0b10 << COM0A0) | (0b11 << WGM00); // Non inverting mode + Configure in fast PWM
  TCCR0B = (5 & 0b111);                        // Set prescaler
  DDRD |= (1 << PD6);                          // Set PD6 / OC0A as output

  DDRD &= ~(1 << PD2); // Configure PD2 as input
  PORTD |= (1 << PD2); // Configure PD2 in pull-up
  DDRB |= (1 << PB5);  // Configure PB5 as output

  while (1) {
    PORTB = (PORTB & ~(1 << PB5)) | (~((PIND & (1 << PD2)) >> PD2) << PB5); // Set PB5 value according to PD2 value

    ADCSRA |= (1 << ADSC); // Start conversion

    while (ADCSRA & (1 << ADSC)) // While there's a conversion
      ;

    OCR0A = ADCH; // Set value of output compare to value of ADConverter
  };
};