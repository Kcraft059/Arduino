#include <avr/io.h>

#define ALC_HDR_SIZE 0x03
#define ALC_HDR_END 0x0fff
#define ALC_SP_PADDING 0x04

.section .data
.global alc_flags
alc_flags: .byte 0b00000001     ; Malloc flag storage 0:first_alloc

.section .bss
mlc_sp_addr: .skip 2

.section .text
.global malloc                  ; MARK: malloc
malloc:                         ; size: r24:r25 & used regs: r18-r21 + Z
  push r16                      ; Store used regs to stack
  push r17

  in r16, _SFR_IO_ADDR(SPL)     ; Load end of available sram (SP)
  in r17, _SFR_IO_ADDR(SPH)
  ldi ZL, lo8(__heap_start)     ; Load start of available sram in Z reg
  ldi ZH, hi8(__heap_start)

  subi r16, ALC_SP_PADDING      ; Apply SP padding 
  sbc r17, r1

  sub r16, ZL                   ; Get available sram which is neither occupied by Stack or Data
  sbc r17, ZH

  cp r24, r16                   ; Compare given size to size of "available" sram 
  cpc r25, r17
  brsh mlc_fail_null            ; Fail if size is too large 

  sts mlc_sp_addr, r16          ; Store padded SP addr
  sts mlc_sp_addr+0x01, r17

  lds r20, alc_flags            ; Load flags from sram
  sbrc r20, 0                   ; Skip if first_alloc flag is cleared (bit 0)
  rjmp mlc_init_header

mlc_heap_walk:                  ; Load metadata (size: r16:r17 offset: r18:r19)
  ld r16, Z+                    ; Load lower 8 bits of size into r16
  ld r18, Z+                    ; Load lower 8 bits of offset into r18
  ld r19, Z+                    ; Load shared size/offset bits
  mov r17, r19
  swap r19
  andi r17, 0b1111              ; Load higher 4 bits of size into r17
  andi r19, 0b1111              ; Load higher 4 bits of offset into r19

  movw r20, r18                 ; Compute real mem size of new alloc 
  subi r20, - ALC_HDR_SIZE      ; Account for header size
  adc r21, r1

  cp r24, r20                   ; Check if offset (free) size is enough
  cpc r25, r21
  brsh mlc_match_free

                                ; MARK: TODO Add check for size:0

  ldi r20, hi8(ALC_HDR_END)     ; Check if reached end of chain (offset = 0xfff)
  cpi r18, lo8(ALC_HDR_END)
  cpc r19, r20
  breq mlc_match_end

  add ZL, r16                   ; Move pointer to next header
  adc ZH, r17
  add ZL, r18
  adc ZH, r19

  lds r16, mlc_sp_addr          ; Check if we did not exceed SP boundary
  lds r17, mlc_sp_addr+0x01
  cp  r16, ZL
  cpc r17, ZH
  brsh mlc_fail_null            ; If SP boundary exceeded 

  rjmp mlc_heap_walk            ; Continue on next header

mlc_fail_null:
  ldi r24, 0x0                  ; Set return val to null ptr
  ldi r25, 0x0

  rjmp mlc_ret

mlc_init_header:                ; Init with flags at r20
  cbr r20, (1 << 0)             ; Clear first_alloc flag
  sts alc_flags, r20            ; Update flags in mem

  ldi r18, lo8(ALC_HDR_END)     ; Set offset to 0xfff to signify end of chain
  ldi r19, hi8(ALC_HDR_END)

  rjmp mlc_new_header

mlc_match_free:                 ; Match for free space in heap fragment & previous size in r16:r17 previous offset in r18:r19 & real size in r20:r21
  sub r18, r20                  ; Set offset = prev_offset - (alloc_size + ALC_HDR_SIZE)
  sbc r19, r21

  rcall mlc_clear_prv_offset

  add ZL, r16                   ; Set ptr to new header 
  adc ZH, r17

  rjmp mlc_new_header

mlc_match_end:                  ; Match for free space at end of heap & previous size in r16:r17
  ldi r18, lo8(ALC_HDR_END)     ; Set offset to 0xfff to signify end of chain
  ldi r19, hi8(ALC_HDR_END)

  rcall mlc_clear_prv_offset

  add ZL, r16                   ; Set ptr to new header 
  adc ZH, r17

mlc_new_header:                 ; New header at addr in Z reg & new size in r24:r25 reg & offset r18:r19
                                ; Set data in header
  andi r19, 0b1111              ; Get only first 4 bits of sizeh 
  andi r25, 0b1111              ; Get only first 4 bits of offset
  swap r19
  or r19, r25                   ; Aggregate high 4 bits of size and high 4 bits of offset in r17
  st Z+, r24                    ; Store lower 8 bits of size in 1st byte of header 
  st Z+, r18                    ; Store lower 8 bits of offset in 2nd byte of header
  st Z+, r19                    ; Store shared size/offset bits

  movw r24, ZL                  ; Set return val to ptr to data

mlc_ret:                        ; Return
  pop r17                       ; Put back used regs & sreg
  pop r16

  ret

mlc_clear_prv_offset:           ; Update previous header
  sbiw ZL, 2                    ; Set data ptr to start of offset low-byte
  st Z+, r1                     ; Set offset to 0
  ld r20, Z                     ; Load shared shared size/offset bits
  andi r20, 0b1111              ; Clear offset bits
  st Z+, r20                    ; Update in mem

  ret

.global free                    ; MARK: free
free:                           ; addr: r25:r24 & used regs: r16:r17
  push r16

  ldi ZL, lo8(__heap_start)     ; Load start of heap addr in Z ptr
  ldi ZH, hi8(__heap_start)

  cp r24, ZL                    ; Compare to start of heap ptr
  cpc r25, ZH
  brlo free_ret                 ; Return if lower
                                ; Check if first_alloc has been made
  lds r16, alc_flags            ; Load flags from sram
  sbrc r16, 0                   ; Skip if first_alloc flag is cleared (bit 0)
  rjmp free_ret

free_heap_walk:                 ; MARK: TODO Implement
free_ret:
  pop r16

  ret

.global malloc_get_header       ; MARK: Debug 
malloc_get_header:
  movw ZL, r24

  sbiw ZL, 3
  ld r22, Z+
  ld r23, Z+
  ld r24, Z+

  ret
