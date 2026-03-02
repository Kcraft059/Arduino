#include <master.h>

#define UPD_LATCH_MASK 0b10000000

/// MARK: PWM SRL
volatile uint8_t srl_data_buf;      // Holds data to be transmitted
volatile uint8_t srl_data_buf_info; // Holds info abt the data buffer

void init_fast_pwm2(uint8_t prescaler) {
  TCCR2A = (0b11 << COM2A0) | (0b11 << WGM20); // Set inverting mode on OC0A & fast pwm mode
  TCCR2B = ((prescaler & 0b111) << CS20);      // Set prescaler
  TIMSK2 = (1 << TOIE2);                       // Enable interrupt overflow
  sei();                                       // Enable global interrupts
};

inline void set_duty_cycle_pwm2(uint8_t duty_cycle) {
  OCR2A = duty_cycle;
}

ISR(TIMER2_OVF_vect) {                      // On overflow
  if (srl_data_buf_info & UPD_LATCH_MASK) { // If latch update queued
    PORTB |= (1 << PB5);                    // Latch to high
    PORTB &= ~((1 << PB4) | (1 << PB5));    // Data & latch to low
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

void send_srl_data(uint8_t data) {
  srl_data_buf = data;   // Store data in buffer
  srl_data_buf_info = 8; // Start send
};
