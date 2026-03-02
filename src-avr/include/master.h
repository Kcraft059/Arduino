#ifndef MAIN_H
#define MAIN_H

#include <lwutils.h>

void init_fast_pwm2(uint8_t prescaler);
void set_duty_cycle_pwm2(uint8_t duty_cycle);
void send_srl_data(uint8_t data);

extern volatile uint8_t srl_data_buf;
extern volatile uint8_t srl_data_buf_info;

void usart_init(uint16_t baud);
void usart_send(uint8_t data);
inline char usart_get(void);
inline uint8_t usart_avail();

#endif