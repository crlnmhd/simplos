#ifndef HAL_H_
#define HAL_H_

#include <stdarg.h>

#include "stdint.h"

void write_mm(uint8_t *const mem_ptr, const uint8_t value);

uint8_t read_mm(uint8_t *const mem_ptr);
uint8_t read_from_mm_adr(const uint16_t mem_adr);

void printf_flash(const char *fmt, ...);

__attribute__((noreturn)) void halt_exec(void);

static inline __attribute__((always_inline)) void disable_interrupts(void) {
  asm volatile("cli" ::: "memory");
}
static inline __attribute__((always_inline)) void enable_interrupts(void) {
  asm volatile("sei" ::: "memory");
}

void setup_interupt_timer(const uint16_t frequenzy_hz);
void init_measurement_ticks(void);
#endif  // HAL_H_
