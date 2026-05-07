#include <avr/io.h>

.section .vectors                     ; Interrupt vector table
.org 0x0000
    jmp main

.section .bss                         ; SRAM Allocation
led_mask: .skip 1                     ; Alloc 1 byte of mem for led_mask

.section .text                     
main:
    ldi r16, lo8(RAMEND)               ; Initialize SP to end of mem
    out _SFR_IO_ADDR(SPL), r16        
    ldi r16, hi8(RAMEND)
    out _SFR_IO_ADDR(SPH), r16

    ldi r16, (1 << PORTB5)             ; Initialize values
    sts led_mask, r16

    ldi r16, (1 << DDB5)               ; Set data direction
    out _SFR_IO_ADDR(DDRB), r16

loop:
    rcall led_toggle                   ; Call led toggling

    ldi r16, 255                       ; Awfull delay method           
delay_0:                                
    ldi r17, 255
delay_1:
    ldi r18, 16
delay_2:
    dec r18
    brne delay_2

    dec r17
    brne delay_1 

    dec r16
    brne delay_0

    rjmp loop                          ; Loop on itself

led_toggle:
    push r16
    push r17
    in r16, _SFR_IO_ADDR(SREG)
    push r16

    in r16 ,_SFR_IO_ADDR(PORTB)       ; Load PORTB state
    lds r17, led_mask                 ; Create bit mask for PORTB5
    eor r16, r17                      ; XOR to toggle led 
    out _SFR_IO_ADDR(PORTB), r16      ; Update PORTB state

    pop r16
    out _SFR_IO_ADDR(SREG), r16
    pop r17
    pop r16

    ret
