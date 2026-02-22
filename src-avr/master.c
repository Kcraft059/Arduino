#include <master.h>
#include <util/delay.h>

#define UPD_LATCH_MASK 0b10000000

volatile uint8_t srl_data_buf;      // Holds data to be transmitted
volatile uint8_t srl_data_buf_info; // Holds info abt the data buffer

void init_fast_pwm0(uint8_t prescaler) {
  TCCR0A = (0b11 << COM0A0) | (0b11 << WGM00); // Set non inverting on OC0A & set fast pwm mode
  TCCR0B = ((prescaler & 0b111) << CS00);      // Set prescaler
  TIMSK0 = /* (1 << OCIE0A) |*/ (1 << TOIE0);  // Set interupts masks
  sei();                                       // Enable global interrupts
};

inline void set_OCA_duty_cyle(uint8_t comp) {
  OCR0A = comp;
};

void send_srl_data(uint8_t data) {
  srl_data_buf = data;
  srl_data_buf_info = 8;
};

void update_latch() {
  srl_data_buf_info = 0; // Reset info
  PORTB |= (1 << PB2);   // Latch to high
  PORTB &= ~(1 << PB2);  // Latch to low
}

ISR(TIMER0_OVF_vect) { // On overflow
  if (srl_data_buf_info & UPD_LATCH_MASK) {
    update_latch();
    PORTB &= ~(1 << PB4); // Turn off data pin
    return;
  }

  if (!(srl_data_buf_info & ~UPD_LATCH_MASK)) // If srl data buffer is empty
    return;

  srl_data_buf_info = (srl_data_buf_info & UPD_LATCH_MASK) | ((srl_data_buf_info & ~UPD_LATCH_MASK) - 1); // Reduce buffer by one

  if (srl_data_buf & (1 << (srl_data_buf_info & ~UPD_LATCH_MASK))) // If 1 in data buffer
    PORTB |= (1 << PB4);
  else
    PORTB &= ~(1 << PB4);

  if (!(srl_data_buf_info & ~UPD_LATCH_MASK)) // At end of counting
    srl_data_buf_info |= (1 << 7);            // Set update mask flag
};

uint8_t count;

int main(void) {
  // Init
  DDRB |= (1 << PB5) | (1 << PB4) | (1 << PB2); // Configure pin 10 (latch) & 12 (srl data) as outputs

  DDRD |= (1 << PD6); // Configure pin 6 (srl clock) as ouput

  init_fast_pwm0(1);      // Config Clock0
  set_OCA_duty_cyle(127); // Init PWM timer

  while (1) {
    send_srl_data(count++); // Send srl data
    _delay_ms(100);
  };
};