#ifndef HAL_H_
#define HAL_H_

#include <stdarg.h>

#include "stdint.h"

void write_mm(uint8_t *const mem_ptr, const uint8_t value);

uint8_t read_mm(uint8_t *const mem_ptr);
uint8_t read_from_mm_adr(const uint16_t mem_adr);

void printf_flash(const char *fmt, ...);

__attribute__((noreturn)) void halt_exec(void);

#endif  // HAL_H_
