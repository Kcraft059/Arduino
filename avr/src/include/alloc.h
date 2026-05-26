/**-----------------------------------------------------------------------*
 * avr-malloc - Lightweight memory allocator for AVR micro-controller
 *
 * Linked list allocator with 3-byte packed headers to minimize overhead.
 *
 * Each header packs 2 values each respectively 12 bits allowing for a
 * maximum theorical single 4094 bytes allocation. (Tho in practice it
 * can't be reached and isn't meant to scale further than 2Kb memory)
 * Header is formated as follows:
 *  - [byte] sizel
 *  - [byte] offsetl
 *  - [byte] sizeh_offseth
 *
 * In header allocated data size is referred as size and offset from next data
 * block as offset.
 * Heap layout:
 *   [header 3b][size Nb][offset Nb][next header 3b]...
 *
 * Limitations:
 *	- Can't work on mem with more than 4096 bytes of mem (as offset would
 *		not be sufficient is heap took all available sram)
 *  - Not interrupt-safe if ISRs use malloc/free they need to do it outside
 *  	of another execution of malloc/free
 *  - Only works on AVR arch (tested on atmega328p)
 *
 * Copyright (C) 2026 @kcraft059 - GPL v3
 *-----------------------------------------------------------------------**/

#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * Allocates a block of memory of the given size.
 *
 * @param size Number of bytes to be allocated (max <= size of available mem <= 4094).
 * @return Pointer to allocated memory, or NULL if allocation failed (size too large, or OOM).
 *
 * @note Not interrupt-safe if ISRs use malloc/free.
 */
extern void* malloc(size_t size);

/**
 * Frees the block of memory at given header.
 *
 * @param ptr Pointer to the adress to be freed.
 *
 * @note Freeing a non-allocated adress is considered as UB.
 */
extern void free(void* ptr);

/**
 * @brief Allocation header
 *
 * Stores size and offset in a 3-byte packed format size and offset being 12 bits each.
 */
struct __attribute__((packed)) alc_header {
  uint8_t sizel;         /**< Lower 8 bits of allocation size */
  uint8_t offsetl;       /**< Lower 8 bits of offset to next block */
  uint8_t sizeh_offseth; /**< High nibbles: [7:4]=offseth [3:0]=sizeh */
};

/**
 * Returns a copy of the header structure at given adress.
 * 
 * @param ptr Pointer to allocated address.
 * @return Allocation header struct.
 *
 * @note Giving a ptr which hasn't been allocated will result in UB.
 */
extern struct alc_header malloc_get_header(void* ptr);
