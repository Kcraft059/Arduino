#include <avr/io.h>

.section .text

.global malloc
malloc:    
  push r16                      ; Store used regs to stack
  push r17
  push r18
  push r19
  in r16, _SFR_IO_ADDR(SREG)     
  push r16             
  
  ; Check size
  in r16, _SFR_IO_ADDR(SPL)     ; Load end of available sram (SP)
  in r17, _SFR_IO_ADDR(SPH)    
  ldi r18, lo8(__data_end)      ; Load start of available sram
  ldi r19, hi8(__data_end)
  sub r16, r18                  ; Get available sram which is neither occupied by Stack or Data
  sbc r17, r19

  cp r24, r16                   ; Compare given size to size of "available" sram 
  cpc r25, r17                  
  brsh malloc_fail              ; Fail if size is too large 

  lds r24, __data_end - 2

  rjmp malloc_ret               ; Return

malloc_fail:            
  ldi r24, 0x0               ; Set return val to null ptr
  ldi r25, 0x0

malloc_ret:             
  pop r16                       ; Put back used regs & sreg
  out _SFR_IO_ADDR(SREG), r16
  pop r19
  pop r18
  pop r17               
  pop r16  
  ret