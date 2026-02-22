#include <master.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

#define UPD_LATCH_MASK 0b10000000

/// MARK: PWM SRL
volatile uint8_t srl_data_buf;      // Holds data to be transmitted
volatile uint8_t srl_data_buf_info; // Holds info abt the data buffer

void init_fast_pwm0(uint8_t prescaler) {
  TCCR0A = (0b11 << COM0A0) | (0b11 << WGM00); // Set inverting mode on OC0A & fast pwm mode
  TCCR0B = ((prescaler & 0b111) << CS00);      // Set prescaler
  TIMSK0 = (1 << TOIE0);                       // Enable interrupt overflow
  sei();                                       // Enable global interrupts
};

void update_latch() {
}

ISR(TIMER0_OVF_vect) {                      // On overflow
  if (srl_data_buf_info & UPD_LATCH_MASK) { // If latch update queued
    PORTB |= (1 << PB2);                    // Latch to high
    PORTB &= ~((1 << PB4) | (1 << PB2));    // Data & latch to low
    srl_data_buf_info = 0;                  // Reset info
    return;
  }

  if (!(srl_data_buf_info & ~UPD_LATCH_MASK)) // If srl data buffer is already empty
    return;

  srl_data_buf_info = (srl_data_buf_info & UPD_LATCH_MASK) | ((srl_data_buf_info & ~UPD_LATCH_MASK) - 1); // Reduce buffer by one

  if (srl_data_buf & (1 << (srl_data_buf_info & ~UPD_LATCH_MASK))) // If 1 in data buffer
    PORTB |= (1 << PB4);
  else
    PORTB &= ~(1 << PB4);

  if (!(srl_data_buf_info & ~UPD_LATCH_MASK)) // If srl data buffer is empty
    srl_data_buf_info |= (1 << 7);            // Set update mask flag
};

inline void set_OCA_duty_cyle(uint8_t comp) {
  OCR0A = comp;
};

void send_srl_data(uint8_t data) {
  srl_data_buf = data;   // Store data in buffer
  srl_data_buf_info = 8; // Start send
};

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
  // Init
  DDRB |= (1 << PB5) | (1 << PB4) | (1 << PB2); // Configure pin 10 (latch) & 12 (srl data) as outputs

  DDRD |= (1 << PD6); // Configure pin 6 (srl clock) as ouput,

  init_fast_pwm0(1);      // Config Clock0
  set_OCA_duty_cyle(127); // Init PWM timer

  DDRD &= ~((1 << PD3) | (1 << PD2)); // Configure 3 (B) et 2 (A) as input
  PORTD |= ((1 << PD3) | (1 << PD2)); // pull-up
  init_rotary_int();

  send_srl_data(nums[9]); // Send srl data

  while (1) {
    if ((count / 4) > 9)
      count = 9 * 4;
    if ((count / 4) < -9)
      count = -9 * 4;

    if (!srl_data_buf_info)
      send_srl_data(nums[abs(count / 4)] | ((count / 4) & (1 << 7)) >> 7);
  };
};