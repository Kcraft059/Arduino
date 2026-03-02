#include <master.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

/// MARK: Clock

volatile void;

void init_clock0(uint8_t prescaler) {
}

/// MARK: Rotary encoder reading

volatile uint8_t status = 0;
volatile int8_t count = 0;

void init_rotary_int() {
  PCICR = (1 << PCIE2);                      // Set int0 as rising edge
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19); // Enable int0
  sei();
};

ISR(PCINT2_vect) {
  status <<= 2;

  if ((~PIND & (1 << PD2))) status |= 0b01; // If A active
  if ((~PIND & (1 << PD3))) status |= 0b10; // If B active

  // Valid transitions
  switch (status & 0b1111) {
  case 0b0001:
  case 0b0111:
  case 0b1110:
  case 0b1000:
    count++; // Clock wise
    break;
  case 0b0010:
  case 0b1011:
  case 0b1101:
  case 0b0100:
    count--;
    break; // Counter clock wise
  }
}

// MARK: ADC

void init_ADC0(uint8_t prescaler) {
  ADCSRA = (1 << ADEN) | ((0b111 & prescaler) << ADPS0);
}

uint16_t read_analog_ADC(uint8_t pinNum) {
  ADMUX = (1 << REFS0) | ((pinNum & 0b1111) << MUX0); // REFS0, AVCC with external capacitor at AREF pin, MUX0 0b0000 input as ADC0
  ADCSRA |= (1 << ADSC);                              // Start conversion

  while (ADCSRA & (1 << ADSC))
    ; // Wait for end of conversion

  return ADCL | ((uint16_t)ADCH << 8);
};

/// MARK: Main

uint8_t nums[] = {
    0b11101110,
    0b00100010,
    0b11010110,
    0b01110110,
    0b00111010,
    0b01111100,
    0b11111100,
    0b00100110,
    0b11111110,
    0b01111110,
};

int main(void) {
  usart_init(9600);

  // Init
  DDRB |= (1 << PB5) | (1 << PB4) | (1 << PB3); // Configure pin 13 (latch) & 12 (srl data) as outputs & 15 (clock)

  init_fast_pwm2(1);        // Config Clock0
  set_duty_cycle_pwm2(127); // Init PWM timer

  // Init rotary
  /* DDRD &= ~((1 << PD3) | (1 << PD2)); // Configure 3 (B) et 2 (A) as input
  PORTD |= ((1 << PD3) | (1 << PD2)); // pull-up
  init_rotary_int(); */

  init_ADC0(0b111);

  DDRC &= ~(1 << PC0);
  DIDR0 |= (1 << ADC0D);
  //PORTC |= (1 << PC0);

  send_srl_data(nums[9]);

  while (1) {
    int value = read_analog_ADC(0);
    float tension = value * 5 / 1023.0; 
    float temp = tension / 0.01;
    
    char buf[64];
    char buf_tens[32];
    char buf_temp[32];

    dtostrf(tension,1,5,buf_tens);
    dtostrf(temp,1,5,buf_temp);

    sprintf(buf, "Valeurs: %d, %s, %s\n", value, buf_tens, buf_temp);

    for (int i = 0; buf[i]; i++)
      usart_send(buf[i]);

    _delay_ms(600);
    /* if ((count / 4) > 9)
      count = 9 * 4;
    if ((count / 4) < -9)
      count = -9 * 4;

    if (!srl_data_buf_info)
      send_srl_data(nums[abs(count / 4)] | ((count / 4) & (1 << 7)) >> 7); */
  };
};