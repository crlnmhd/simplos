#define _GNU_SOURCE
#ifndef MEMORY_H_
#define MEMORY_H_
#include "memory_layout.h"
#include "simplos_types.h"
#include "stdint.h"

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
