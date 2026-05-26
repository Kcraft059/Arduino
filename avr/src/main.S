/**-----------------------------------------------------------------------*
 * avr-init - Initialization program for atmega328p 
 *
 * Initializes SP, clears r1 and copy static .data
 *
 * Copyright (C) 2026 @kcraft059 - GPL v3
 *-----------------------------------------------------------------------**/

#include <avr/io.h>

.section .vectors                        ; Interrupt vector table
  jmp init

.section .text
init:
  clr r1                                 ; Zero r1 reg

  ldi r16, lo8(RAMEND)                   ; Init Stack pointer
  out _SFR_IO_ADDR(SPL), r16
  ldi r16, hi8(RAMEND)
  out _SFR_IO_ADDR(SPH), r16

  ldi ZL, lo8(__data_load_start)         ; Load flash data adress in Z reg
  ldi ZH, hi8(__data_load_start)
  ldi XL, lo8(__data_start)              ; Load sram start adress in X reg
  ldi XH, hi8(__data_start)
  ldi r17, lo8(__data_end)               ; Load sram end adress in r17-18 reg
  ldi r18, hi8(__data_end)
  rjmp data_copy_cmp                     ; Start data copy

data_copy_loop:
  lpm r0, Z+                             ; Read data from flash
  st X+, r0                              ; Load data in sram 

data_copy_cmp:
  cp XL, r17                             ; Compare X (16 bit) to see if we reached end of data section
  cpc XH, r18
  brlo data_copy_loop                    ; If X is in the data range, continue loop     

  rcall c_entry                          ; Program start

end:
  rjmp end                               ; End loop
