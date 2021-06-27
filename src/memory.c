#include "memory.h"

#include <avr/io.h>
#include <stdint.h>

#include "interupts.h"
#include "io_helpers.h"
#include "simplos_types.h"

uint8_t heap_mapping[ALLOCABLE_MEMORY_BYTES / HEAP_PAGE_SIZE];

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
  assert(memory_region(task) == TASK_RAM,
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
    case 0 ... 0x1FF:
      region = REGISTERS;
      break;
    case 0x200 ... 0x2FF:
      region = STATIC_RAM;
      break;
    case 0x300 ... 0x349:
      region = OS_STACK;
      break;
    case 0x350 ... 0xD49:
      region = TASK_RAM;
      break;
    case 0xD50 ... 0x2000:
      region = HEAP;
      break;
    default:
      region = UNKNOWN;
  }
  return region;
}

uint16_t init_heap(void) {
  assert(os_stack_start() == 0x349, "Incorrect os stack start");
  assert(stack_end() == 0x350, "Incorrect stack end");
  assert(HEAP_RAM_END == 0xD50, "Incorrect heap low");
  return 0;
}
