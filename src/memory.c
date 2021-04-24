#include "memory.h"

#include <avr/io.h>
#include <stdint.h>

#include "interupts.h"
#include "io_helpers.h"
#include "simplos_types.h"

uint16_t ram_start = RAMSTART + 0x500;

uint16_t task_default_sp(uint8_t task_memory_block) {
  // Stack grows toward smaller values.
  uint16_t const sp_adr =
      ram_start + (task_memory_block + 1) * TASK_MEMORY_BYTES - 1;
  return sp_adr;
}

uint16_t stack_end() { return ram_start; }