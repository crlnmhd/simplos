#define _GNU_SOURCE
#ifndef MEMORY_H_
#define MEMORY_H_
#include <stdint.h>

#include "memory_layout.h"
#include "simplos_types.h"

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

uint16_t task_default_sp(uint8_t task_index);

void assert_task_pointer_integrity(taskptr_type task_block);

uint16_t task_memory_size(void);

void check_task_configuration_uses_all_available_memory(void);

void configure_heap_location(const uint8_t margin_to_main);
#endif  // MEMORY_H_
