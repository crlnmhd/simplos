#ifndef MEMORY_H_
#define MEMORY_H_

#include "simplos_types.h"
#include "stdint.h"

// End of main and some extra just to be sure.
#define HEAP_START 0x2000
#define HEAP_PAGE_SIZE 64

#define TASK_MEMORY_BYTES 512
#define ALLOCABLE_MEMORY_BYTES 1024

// OS task memory for use within constext switches and alike. Aprox 50 bytes).
// RAMSTART = 0x200
#define OS_STACK_SIZE 0x40
#define MEM_START 0x200
#define MAX_STATIC_RAM_USAGE 0x100
#define TASK_RAM_END (MEM_START + MAX_STATIC_RAM_USAGE + OS_STACK_SIZE)

extern uint8_t* heap_map;

uint16_t task_default_sp(uint8_t);
uint16_t stack_end(void);
// uint16_t os_stack_start(void);
// uint16_t os_stack_end(void);

void assert_stack_integrity(taskptr_t task_block);

uint16_t init_heap(void);

#endif  // MEMORY_H_
