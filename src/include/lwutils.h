#ifndef LWUTILS_H
#define LWUTILS_H

#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <avr/io.h>

// USART
void usart_init(uint16_t baud);
void usart_send(uint8_t data);
inline char usart_get();
inline uint8_t usart_avail();

// EEPROM
uint8_t eeprom_read_byte(uint16_t addr);
void eeprom_erase_byte(uint16_t addr);
void eeprom_write_byte(uint16_t addr, uint8_t data);

#endif