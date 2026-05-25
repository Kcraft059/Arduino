#include <avr/io.h>

#define ALC_HDR_SIZE 0x03
#define ALC_HDR_END 0x0fff
#define ALC_SP_PADDING 0x04

.section .data
.global alc_flags
alc_flags: .byte 0b00000001                           ; Malloc flag storage 0:first_alloc

.section .bss
mlc_sp_addr: .skip 2

.section .text
.global malloc                                        ; MARK: malloc
malloc:                                               ; size: r24:r25 & used regs: r18-r21 + Z
  push r16                                            ; Store used regs to stack
  push r17

  in r16, _SFR_IO_ADDR(SPL)                           ; Load end of available sram (SP)
  in r17, _SFR_IO_ADDR(SPH)
  ldi ZL, lo8(__heap_start)                           ; Load start of available sram in Z reg
  ldi ZH, hi8(__heap_start)

  subi r16, ALC_SP_PADDING                            ; Apply SP padding 
  sbc r17, r1

  sub r16, ZL                                         ; Get available sram which is neither occupied by Stack or Data
  sbc r17, ZH

  cp r24, r16                                         ; Compare given size to size of "available" sram 
  cpc r25, r17
  brsh mlc_fail_null                                  ; Fail if size is too large 

  sts mlc_sp_addr, r16                                ; Store padded SP addr
  sts mlc_sp_addr+0x01, r17

  lds r20, alc_flags                                  ; Load flags from sram
  sbrc r20, 0                                         ; Skip if first_alloc flag is cleared (bit 0)
  rjmp mlc_init_header

mlc_heap_walk:                                        ; Load metadata (size: r16:r17 offset: r18:r19)
  ld r16, Z+                                          ; Load lower 8 bits of size into r16
  ld r18, Z+                                          ; Load lower 8 bits of offset into r18
  ld r19, Z+                                          ; Load shared size/offset bits
  mov r17, r19
  swap r19
  andi r17, 0b1111                                    ; Load higher 4 bits of size into r17
  andi r19, 0b1111                                    ; Load higher 4 bits of offset into r19

	cp r16, r1                                          ; Check if size is 0
	cpc r17, r1
	brne mlc_heap_walk_continue                         ; If not 0, continue heap walk
	
	cp r18, r24                                         ; Compare offset to size without header size
	cpc r19, r25
	brsh mlc_match_nullsize

mlc_heap_walk_continue:
  cpi r18, lo8(ALC_HDR_END)                           ; Check if reached end of chain (offset = 0xfff)
  ldi r20, hi8(ALC_HDR_END)
  cpc r19, r20
  breq mlc_match_end

  movw r20, r24                                       ; Compute real mem size of new alloc 
  subi r20, lo8(-ALC_HDR_SIZE)                        ; Account for header size
  sbci r21, hi8(-ALC_HDR_SIZE)                        ; Cursed substraction magic r20 - (-ALC_HDR_SIZE) = r20 + ALC_HDR_SIZE

  cp r18, r20                                         ; Check if offset (free) size is enough (with header size)
  cpc r19, r21
  brsh mlc_match_free
	
  add ZL, r16                                         ; Move pointer to next header
  adc ZH, r17
  add ZL, r18
  adc ZH, r19

  lds r16, mlc_sp_addr                                ; Check if we did not exceed SP boundary
  lds r17, mlc_sp_addr+0x01
  cp  ZL, r16
  cpc ZH, r17
  brsh mlc_fail_null                                  ; If SP boundary exceeded 

  rjmp mlc_heap_walk                                  ; Continue on next header

mlc_fail_null:
  ldi r24, 0x0                                        ; Set return val to null ptr
  ldi r25, 0x0

  rjmp mlc_ret

mlc_init_header:                                      ; Init with flags at r20
  cbr r20, (1 << 0)                                   ; Clear first_alloc flag
  sts alc_flags, r20                                  ; Update flags in mem

  ldi r18, lo8(ALC_HDR_END)                           ; Set offset to 0xfff to signify end of chain
  ldi r19, hi8(ALC_HDR_END)

  rjmp mlc_new_header

mlc_match_nullsize:                                   ; Match for free space in 0sized header & previous offset in r18:r19 & alloc size in r24:r25
	sub r18, r24                                        ; Set offset = prev_offset - alloc_size
	sbc r19, r25

	sbiw ZL, 3                                          ; Set ptr to start of nullsized header

  rjmp mlc_new_header                                 ; Overwrite 0sized header

mlc_match_end:                                        ; Match for free space at end of heap & previous size in r16:r17
  ldi r18, lo8(ALC_HDR_END)                           ; Set offset to 0xfff to signify end of chain
  ldi r19, hi8(ALC_HDR_END)

  rcall mlc_clear_prv_offset

  add ZL, r16                                         ; Set ptr to new header 
  adc ZH, r17

  rjmp mlc_new_header

mlc_match_free:                                       ; Match for free space in heap fragment & previous size in r16:r17 previous offset in r18:r19 & real size in r20:r21
  sub r18, r20                                        ; Set offset = prev_offset - (alloc_size + ALC_HDR_SIZE)
  sbc r19, r21

  rcall mlc_clear_prv_offset

	add ZL, r16
  adc ZH, r17

mlc_new_header:                                       ; New header at addr in Z reg & alloc size in r24:r25 reg & offset r18:r19
                                                      ; Set data in header
  andi r19, 0b1111                                    ; Get only first 4 bits of sizeh 
  andi r25, 0b1111                                    ; Get only first 4 bits of offset
  swap r19
  or r19, r25                                         ; Aggregate high 4 bits of size and high 4 bits of offset in r17
  st Z+, r24                                          ; Store lower 8 bits of size in 1st byte of header 
  st Z+, r18                                          ; Store lower 8 bits of offset in 2nd byte of header
  st Z+, r19                                          ; Store shared size/offset bits

  movw r24, ZL                                        ; Set return val to ptr to data

mlc_ret:                                              ; Return addr in r24:r25
  pop r17                                             ; Put back used regs & sreg
  pop r16

  ret

mlc_clear_prv_offset:                                 ; Update previous header
  sbiw ZL, 2                                          ; Set data ptr to start of offset low-byte
  st Z+, r1                                           ; Set offset to 0
  ld r20, Z                                           ; Load shared shared size/offset bits
  andi r20, 0b1111                                    ; Clear offset bits
  st Z+, r20                                          ; Update in mem

  ret

.global free                                          ; MARK: free
free:                                                 ; addr: r25:r24 & used regs: r16:r17
  push r16
  push r17

  ldi ZL, lo8(__heap_start)                           ; load start of heap addr
  ldi ZH, hi8(__heap_start)

  cpi r24, lo8(__heap_start + ALC_HDR_SIZE)           ; Compare to first possible data ptr
	ldi r20, hi8(__heap_start + ALC_HDR_SIZE)
	cpc r25,r20
  brlo free_ret                                       ; Return if lower
	breq free_first_header                              ; If addr given is first data ptr, then we should act on first header
                                                      ; Check if first_alloc has been made
  lds r16, alc_flags                                  ; Load flags from sram
  sbrc r16, 0
  rjmp free_ret                                       ; Return is first_alloc flag is set

free_heap_walk:
  ld r16, Z+                                          ; Load lower 8 bits of size into r16
  ld r18, Z+                                          ; Load lower 8 bits of offset into r18
  ld r19, Z+                                          ; Load shared size/offset bits
  mov r17, r19
  swap r19
  andi r17, 0b1111                                    ; Load higher 4 bits of size into r17
  andi r19, 0b1111                                    ; Load higher 4 bits of offset into r19

	cp r24, ZL                                          ; Compare input addr to current data addr
	cpc r25, ZH
	breq free_match                                     ; If addr match, free current data 

	cpi r18, lo8(ALC_HDR_END)                           ; Check if given header has ALC_HDR_END code as offset
	ldi r20, hi8(ALC_HDR_END)
	cpc r19, r20
	breq free_ret                                       ; Fail if we reached end of chain

	movw r22, ZL                                        ; Store last data ptr addr in r22:r23
	subi r22, 0x3                                       ; Set last data ptr addr to start of its header
	sbc r23, r1

	add ZL, r16                                         ; Set ptr to start of next header
	adc ZH, r17
	add ZL, r18
	adc ZH, r19

	rjmp free_heap_walk

free_match:
	movw ZL, r22                                        ; Set ptr to start of last header;
	
	cpi r18, lo8(ALC_HDR_END)                           ; Check if given header has ALC_HDR_END code as offset
	ldi r20, hi8(ALC_HDR_END)
	cpc r19, r20
	breq free_match_end

	ldd r22, Z+1                                        ; Load lower 8 bits of offset into r22
	ldd r23, Z+2                                        ; Load shared size/offset bits
	swap r23
	andi r23, 0b1111                                    ; Load higher 4 bits of offset into r23

	ldi r20, lo8(ALC_HDR_SIZE)                          ; Set new_offset = offset + prev_hdr_offset + prev_hdr_size + ALC_HDR_SIZE
	ldi r21, hi8(ALC_HDR_SIZE)
	add r22, r20 
	adc r23, r21
	add r22, r16
	adc r23, r17
	add r22, r18
	adc r23, r19

free_match_hdr_set:                                   ; Set hdr offset of hdr at addr Z to r22:r23 
	ldd r16, Z+2

	andi r16, 0b1111
	andi r23, 0b1111
	swap r23
	or r23,r16
	std Z+1, r22                                        ; Store lower 8 bits of offset into r22
	std Z+2, r23

free_ret:                                             ; Returns nothing
  pop r17
  pop r16

  ret

free_first_header:
	ld r16, Z                                           ; Load lower 8 bits of size into r16 
	ldd r18, Z+1                                        ; Load lower 8 bits of offset into r18
	ldd r17, Z+2                                        ; Load shared offset / size bits
	mov r19, r17
	swap r19
	andi r17, 0b1111                                    ; Load higher 4 bits of size into r17
	andi r19, 0b1111                                    ; Load higher 4 bits of offset into r19
	
	add r18, r16                                        ; Set new_offset = offset + size
	adc r19, r16

	andi r19, 0b1111                                    ; Zero higher 4 bits of size
	swap r19
	st Z, r1                                            ; Zero lower 8 bits of size
	std Z+1, r18                                        ; Store lower 8 bits of new offset
	std Z+2, r19                                        ; Store higher 4 bits of new offset

	rjmp free_ret

free_match_end:                                       ; Set new_offset to ALC_HDR_END code
	ldi r22, lo8(ALC_HDR_END)
	ldi r23, hi8(ALC_HDR_END)

	rjmp free_match_hdr_set

.global malloc_get_header                             ; MARK: Debug 
malloc_get_header:
  movw ZL, r24

  sbiw ZL, 3
  ld r22, Z+
  ld r23, Z+
  ld r24, Z+

  ret
