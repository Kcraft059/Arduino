#include <avr/io.h>

.section .vectors                     ; Interrupt vector table
.org 0x0000
    jmp main
.org 0x0004 
    jmp ledOn                         ; INT0 vector

.section .text                        ; The actual code section
main:
    ldi r16, (1 << DDB5)              ; DataDirection for portb5 to out
    out _SFR_IO_ADDR(DDRB), r16

    ldi r16, (0b11 << ISC00)          ; Interrupt on rising edge
    sts EICRA, r16      
    ldi r16, (1 << INT0)              ; Enable INT0
    out _SFR_IO_ADDR(EIMSK), r16      
    sei                               ; Enable global interrupts
    jmp loop

ledOn:
    ldi r17, (1 << PORTB5)            ; Toggle mask
    in  r16, _SFR_IO_ADDR(PORTB)      ; Get portb state
    eor r16, r17                      ; Xor to toggle pb5 state
    out _SFR_IO_ADDR(PORTB), r16      
    reti                              ; Return from interrupt

loop:
    rjmp loop                         ; Infinite loop — do nothing forever