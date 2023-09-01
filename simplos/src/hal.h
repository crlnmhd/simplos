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

uint16_t get_tick_counter(void);  // TODO: inline mem access for performance?
void clear_tick_counter(void);

#define CLEAR_IO_REG(io_reg)                                               \
  asm volatile("sts  %[reg], __zero_reg__  \n\t" ::[reg] "M"(_SFR_IO_ADDR( \
      io_reg)));  // TODO: avoid relying on macro. Direct mem access?

#endif  // HAL_H_
