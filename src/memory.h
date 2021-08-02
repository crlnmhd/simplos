#define _GNU_SOURCE
#ifndef MEMORY_H_
#define MEMORY_H_
#include "simplos_types.h"
#include "stdint.h"

/*
 * 0x0-1FF            # Registers
 * 0x200 - 0x2FF      # Static ram
 * 0x300 - 0x34F      # OS stack
 * 0x350 - 0xD4F      # (5) task stack
 * 0xD50 - 0x2000     # heap
 */

#define TASK_MEMORY_BYTES 512
#define ALLOCABLE_MEMORY_BYTES 2048

// End of main and some extra just to be sure.
#define HEAP_PAGE_SIZE 256

#define HEAP_CHUNKS (ALLOCABLE_MEMORY_BYTES / HEAP_PAGE_SIZE)
// OS task memory for use within constext switches and alike. Aprox 50 bytes).
// RAMSTART = 0x200
// Size of the heap map.
#define HEAP_START (0x2000 - HEAP_CHUNKS)

#define OS_STACK_SIZE 0x50
#define MEM_START 0x200
#define MAX_STATIC_RAM_USAGE 0x100
#define TASK_RAM_LOW (MEM_START + MAX_STATIC_RAM_USAGE + OS_STACK_SIZE)
#define HEAP_RAM_END (TASK_RAM_LOW + (TASKS_MAX * TASK_MEMORY_BYTES))

enum MEM_REGION {
  REGISTERS,
  STATIC_RAM,
  OS_STACK,
  TASK_RAM,
  HEAP,
  HEAP_MAP,
  UNKNOWN,
};

enum MEM_REGION memory_region(taskptr_t);

uint16_t task_default_sp(uint8_t);
uint16_t stack_end(void);
uint16_t os_stack_start(void);
// uint16_t os_stack_end(void);

void assert_stack_integrity(taskptr_t task_block);

#endif  // MEMORY_H_
