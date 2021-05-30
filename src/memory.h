#ifndef MEMORY_H_
#define MEMORY_H_

#include "simplos_types.h"
#include "stdint.h"

// End of main and some extra just to be sure.
#define HEAP_START 0x2100
#define HEAP_PAGE_SIZE 64

#define TASK_MEMORY_BYTES 512
#define ALLOCABLE_MEMORY_BYTES 1024

// OS task memory for use within constext switches and alike. Aprox 50 bytes).
// RAMSTART = 0x200
#define OS_RAM_START (0x200 + 0x149)
#define TASK_RAM_END (0x200 + 0x150)

extern uint8_t* heap_map;

uint16_t task_default_sp(uint8_t);
uint16_t stack_end(void);
uint16_t os_stack_start(void);
uint16_t os_stack_end(void);

void assert_stack_integrity(taskptr_t task_block);

uint16_t init_heap(void);

#endif  // MEMORY_H_
