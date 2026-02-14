#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <digit.h>
#include <serial_interpreter.h>

extern SerialInterpreter* srl_inter;
extern Digit* digit_display;

// Serial commands
uint8_t charCmd(char**);
uint8_t rawCmd(char**);
uint8_t brightnessCmd(char**);
uint8_t stringCmd(char**);

#endif