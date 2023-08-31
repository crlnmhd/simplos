#include "hal.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

void write_mm(uint8_t *const mem_ptr, const uint8_t value) { *mem_ptr = value; }

uint8_t read_mm(uint8_t *const mem_ptr) { return *mem_ptr; }

uint8_t read_from_mm_adr(const uint16_t mem_adr) {
  return read_mm((uint8_t *)mem_adr);
}

void printf_flash(const char *fmt, ...) {
  uint8_t sreg_at_entry = SREG;
  disable_interrupts();
  va_list args;
  va_start(args, fmt);
  vfprintf_P(stdout, fmt, args);
  va_end(args);
  SREG = sreg_at_entry;  // also restores interupt flag
}

__attribute__((noreturn)) void halt_exec(void) {
  disable_interrupts();
  for (;;) {
  }
  __builtin_unreachable();
}
