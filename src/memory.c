#include "memory_layout.h"
#define _GNU_SOURCE
#include <avr/io.h>
#include <stdint.h>

#include "io_helpers.h"
#include "memory.h"
#include "simplos_types.h"
#include "timers.h"

uint16_t task_default_sp(uint8_t const task_memory_block) {
  if (task_memory_block >= TASKS_MAX) {
    fatal_error("(task_default_sp()): Memory block '%d' is INVALID!",
                task_memory_block);
  }
  // Stack grows toward smaller values.
  uint16_t const sp_adr =
      TASK_RAM_LOW + ((task_memory_block + 1) * TASK_MEMORY_BYTES) - 1;
  // dprint("Giving task %d SP 0x%X\n", task_memory_block, sp_adr);
  return sp_adr;
}

uint16_t stack_end() { return TASK_RAM_LOW; }
// Stack grows downwards.

uint16_t os_stack_start(void) { return TASK_RAM_LOW - 1; }

// Stack grows downwards.
uint16_t os_stack_end(void) {
  uint16_t const last_start = task_default_sp(TASKS_MAX - 1);
  uint16_t const margin = 0x0;  // Update memory layout on change!

  return last_start - margin;
}

void assert_stack_integrity(taskptr_t task) {
  cprint("task SP: 0x%X\n", task->task_sp_adr);
  ASSERT_EQ(memory_region(task), TASK_RAM, "0x%X",
            "MEMORY ERROR! Task pointer outside task pointer region!");
  uint16_t const upper_bound = task_default_sp(task->task_memory_block);
  uint16_t const lower_bound =
      task->task_memory_block == 0
          ? stack_end()
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

enum MEM_REGION memory_region(taskptr_t adr) {
  enum MEM_REGION region;
  // Switch on range is a very usefull GCC extension :-)
  switch ((uint16_t)adr->task_sp_adr) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    case 0 ... 0x1FF:
      region = REGISTERS;
      break;
    case 0x200 ... 0x200 + MAX_STATIC_RAM_USAGE - 1:
      region = STATIC_RAM;
      break;
    case 0x200 + MAX_STATIC_RAM_USAGE... 0x200 + MAX_STATIC_RAM_USAGE +
        OS_STACK_SIZE - 1:
      region = OS_STACK;
      break;
    case 0x200 + MAX_STATIC_RAM_USAGE + OS_STACK_SIZE... 0xE4F:
      region = TASK_RAM;
      break;
    case 0xE50 ... HEAP_START:
      region = HEAP;
      break;
    case HEAP_START + 1 ... 0x2000:
      region = HEAP_MAP;
      break;
    default:
      region = UNKNOWN;
#pragma GCC diagnostic pop
  }
  return region;
}
