#include "hal.h"

void write_mm(uint8_t *const mem_ptr, const uint8_t value) { *mem_ptr = value; }

uint8_t read_mm(uint8_t *const mem_ptr) { return *mem_ptr; }

uint8_t read_mm_adr(const uint16_t mem_adr) {
  return read_mm((uint8_t *)mem_adr);
}
