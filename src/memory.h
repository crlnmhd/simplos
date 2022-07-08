#define _GNU_SOURCE
#ifndef MEMORY_H_
#define MEMORY_H_
#include "memory_layout.h"
#include "simplos_types.h"
#include "stdint.h"

enum MEM_REGION {
  REGISTERS = 0,
  REGISTERS_CANARY,
  OS_STACK,
  TASK_RAM,
  HEAP,
  INIT,
  UNKNOWN,
};

enum MEM_REGION memory_region(taskptr_type);

uint16_t task_default_sp(uint8_t);

void assert_stack_integrity(taskptr_type task_block);

#endif  // MEMORY_H_
