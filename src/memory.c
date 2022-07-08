#include "memory_layout.h"
#include "simplos.h"
#define _GNU_SOURCE
#include <avr/io.h>
#include <stdint.h>

#include "io_helpers.h"
#include "memory.h"
#include "simplos_types.h"
#include "timers.h"

INLINED bool in_region(size_t address, size_t region_start, size_t region_end) {
  return region_end <= address && address <= region_start;
}

uint16_t task_default_sp(uint8_t const task_memory_block) {
  if (task_memory_block >= TASKS_MAX) {
    fatal_error("(task_default_sp()): Memory block '%d' is INVALID!",
                task_memory_block);
  }
  // Stack grows toward smaller values.
  uint16_t const sp_adr =
      TASK_RAM_END + ((task_memory_block + 1) * TASK_MEMORY_BYTES) - 1;
  // dprint("Giving task %d SP 0x%X\n", task_memory_block, sp_adr);
  return sp_adr;
}

void assert_stack_integrity(taskptr_type task) {
  cprint("Task SP: 0x%X\n", task->task_sp_adr);
  ASSERT_EQ(memory_region(task), TASK_RAM, "0x%X",
            "MEMORY ERROR! Task pointer outside task pointer region!");

  for (uint8_t *canary_byte = (uint8_t *)CANARY_START;
       canary_byte >= (uint8_t *)CANARY_END; canary_byte--) {
    ASSERT_EQ(*canary_byte, CANARY_VALUE, "0x%X",
              "Canary has been changed! The OS stack has likely overflown\n");
  }
  uint16_t const upper_bound = task_default_sp(task->task_memory_block);
  uint16_t const lower_bound =
      task->task_memory_block == 0
          ? OS_STACK_START + 1
          : task_default_sp(task->task_memory_block - 1);
  bool const sp_within_bounds =
      (lower_bound <= task->task_sp_adr && task->task_sp_adr <= upper_bound);

  if (!sp_within_bounds) {
    cprint("Current task sp: 0x%X and block: %d\nUpper: 0x%Xd, lower: 0x%X\n",
           task->task_sp_adr, task->task_memory_block, upper_bound,
           lower_bound);
    fatal_error("STACK OVERFLOW DETECTED!\nTask %d SP = 0x%X is of bounds.",
                task->task_memory_block, task->task_sp_adr);
  }
}

enum MEM_REGION memory_region(taskptr_type adr) {
  const size_t sp = adr->task_sp_adr;
  if (in_region(sp, REGISTERS_START, 0)) {
    return REGISTERS;
  } else if (in_region(sp, CANARY_START, CANARY_END)) {
    return REGISTERS_CANARY;
  } else if (in_region(sp, OS_STACK_START, OS_STACK_END)) {
    return OS_STACK;
  } else if (in_region(sp, TASK_RAM_START, TASK_RAM_END)) {
    return TASK_RAM;
  } else if (in_region(sp, kernel->heap_start, HEAP_END)) {
    return HEAP;
  } else if (in_region(sp, RAMEND, kernel->heap_start + 1)) {
    return INIT;
  } else {
    return UNKNOWN;
  }
}
