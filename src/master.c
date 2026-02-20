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
  //usart_init(9600);

  init_ADC(7);
  DDRC &= ~(1 << PC0);   // Set PC0 as input
  DIDR0 |= (1 << ADC0D); // Disable digital on pin ADC0

  init_CL0(1);
  DDRD |= (1 << PD6); // Set PD6 / OC0A as output

  while (1) {
    uint16_t value = read_analog_ADC(0);
    set_cmp_OC0A(value / 4);

    /* char strbuf[] = "Value:0000\n";
    utoa(value, strbuf + 6, 10);
    strbuf[strlen(strbuf) + 1] = '\0';
    strbuf[strlen(strbuf)] = '\n';

    for (uint8_t i = 0; strbuf[i]; i++)
      usart_send(strbuf[i]);

    _delay_ms(100); */
  };
};