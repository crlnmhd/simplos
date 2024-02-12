#define _GNU_SOURCE
#include "memory.hpp"

#include <stdint.h>

#include "hal/hal.hpp"
#include "io_helpers.hpp"
#include "memory_layout.hpp"
#include "simplos.hpp"
#include "simplos_types.hpp"

void verify_canaries(void);
bool mem_adr_belongs_to_task(uint16_t adr, uint8_t task_number, Kernel *kernel);

void configure_heap_location(const uint8_t margin_to_main, Kernel *kernel) {
  kernel->heap_start = SP - margin_to_main;
  debug_print("Heap starting at  0x%X\n", kernel->heap_start);
  ASSERT(kernel->heap_start > task_ram_start,
         "init section has overflowed heap memory");
  debug_print("%u bytes available for heap.\n",
              kernel->heap_start - task_ram_start);
}

void warn_if_task_memory_can_not_be_divided_evenly_between_tasks(void) {
  // Warn if not all available task memory has been allocated due to sub-optimal
  // configuration.
  constexpr uint16_t available_task_stack_size = task_ram_start - task_ram_end;
  constexpr uint16_t used_task_memory = task_memory_size * tasks_max;
  if (used_task_memory != available_task_stack_size) {
    WARNING(
        "Could not divide available stack evenly between tasks. Consider "
        "modifying the configuration.\n");
  }
}

INLINED bool in_region(size_t address, size_t region_start, size_t region_end) {
  return region_end <= address && address <= region_start;
}

uint16_t task_sp_range_high(uint8_t const task_memory_block) {
  constexpr uint16_t index_increments =
      task_memory_size - 1U;  // Task stack begins at increment 0, so max
                              // increment is TASK_MEMORY_SIZE -1
  return task_sp_range_low(task_memory_block) + index_increments;
}

/*
 * Points to the first byte of 'task_memory_block', i.e. the one following
 * [start of current task sp = 0], 1, ... TASK_MEMORY_SIZE - 1.
 * */
uint16_t task_sp_range_low(uint8_t const task_memory_block) {
  if (task_memory_block >= tasks_max) {
    FATAL_ERROR("(task_sp_rane_high()): Memory block '%u' is INVALID!",
                task_memory_block);
  }
  return task_ram_end + (task_memory_size * task_memory_block);
}

void assert_task_pointer_integrity(Simplos_Task *task, Kernel *kernel) {
  ASSERT_EQ(memory_region(task, kernel), MEM_REGION::TASK_RAM, "0x%X",
            "MEMORY ERROR! Task pointer outside task pointer region!");

  ASSERT(mem_adr_belongs_to_task(task->task_sp_adr, task->task_memory_block,
                                 kernel),
         "TASK MEMORY ERROR! Saved stack pointer is outside allowed range "
         "for task\n.");

  const uint16_t future_task_sp_adr_with_saved_registers =
      task->task_sp_adr - num_context_switch_overhead_bytes();
  ASSERT(mem_adr_belongs_to_task(future_task_sp_adr_with_saved_registers,
                                 task->task_memory_block, kernel),
         "(Future) TASK MEMORY ERROR! Future stack pointer, with saved "
         "registers is outside allowed range for task\n.");

  verify_canaries();
  uint16_t const upper_bound = task_sp_range_high(task->task_memory_block);
  uint16_t const lower_bound = task_sp_range_low(task->task_memory_block);

  bool const sp_outside_bounds =
      task->task_sp_adr < lower_bound || task->task_sp_adr > upper_bound;

  if (sp_outside_bounds) {
    debug_print(
        "Current task sp: 0x%X and block: %u\nUpper: 0x%X, lower: 0x%X\n",
        task->task_sp_adr, task->task_memory_block, upper_bound, lower_bound);
    FATAL_ERROR("STACK OVERFLOW DETECTED!\nTask %u SP = 0x%X is of bounds.",
                task->task_memory_block, task->task_sp_adr);
  }
}
bool mem_adr_belongs_to_task(uint16_t adr, uint8_t task_memory_block,
                             Kernel *kernel) {
  struct MemorySpan task_stack_range =
      kernel->task_RAM_ranges[task_memory_block];

  return task_stack_range.low <= adr && adr <= task_stack_range.high;
}
void verify_canaries(void) {
  /* Verifies the canary bytes between the OS stack and the first task. */
  for (uint8_t *canary_byte = (uint8_t *)canary_start;
       canary_byte >= (uint8_t *)canary_end; canary_byte--) {
    ASSERT_EQ(read_mm(canary_byte), canary_value, "0x%X",
              "Canary has been changed! The OS stack has likely overflown\n");
  }
}

enum MEM_REGION memory_region(const Simplos_Task *adr, Kernel *kernel) {
  const size_t sp = adr->task_sp_adr;
  if (in_region(sp, registers_start, 0)) {
    return MEM_REGION::REGISTERS;
  } else if (in_region(sp, canary_start, canary_end)) {
    return MEM_REGION::REGISTERS_CANARY;
  } else if (in_region(sp, os_stack_start, os_stack_end)) {
    return MEM_REGION::OS_STACK;
  } else if (in_region(sp, task_ram_start, task_ram_end)) {
    return MEM_REGION::TASK_RAM;
  } else if (in_region(sp, kernel->heap_start, heap_end)) {
    return MEM_REGION::HEAP;
  } else if (in_region(sp, RAMEND, kernel->heap_start + 1)) {
    return MEM_REGION::INIT;
  } else {
    return MEM_REGION::UNKNOWN;
  }
}
