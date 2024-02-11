#define _GNU_SOURCE
#ifndef MEMORY_H_
#define MEMORY_H_
#include <stdint.h>

#include "simplos_types.hpp"

enum class MEM_REGION {
  REGISTERS,
  REGISTERS_CANARY,
  OS_STACK,
  TASK_RAM,
  HEAP,
  INIT,
  UNKNOWN,
};

enum MEM_REGION memory_region(const Simplos_Task *, Kernel *kernel);

uint16_t task_sp_range_high(uint8_t task_index);

uint16_t task_sp_range_low(uint8_t task_index);

void assert_task_pointer_integrity(Simplos_Task *task_block, Kernel *kernel);

void warn_if_task_memory_can_not_be_divided_evenly_between_tasks(void);

void configure_heap_location(const uint8_t margin_to_main, Kernel *kernel);
#endif  // MEMORY_H_
