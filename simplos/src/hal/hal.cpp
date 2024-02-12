#include "hal.hpp"
#ifdef MOCK_HAL

/*
 * Mock hall used for testing
 * */

struct HardwareState state = {.interrupt_enabled = false, .halted = false};
void halt_exec(void) { state.halted = true; }
void enable_interrupts(void) { state.interrupt_enabled = true; }
void disable_interrupts(void) { state.interrupt_enabled = false; }

/* Placeholders*/
void write_mm(uint8_t *const mem_ptr, const uint8_t value) {}
uint8_t read_mm(uint8_t *const mem_ptr) { return 0xFF; }
uint8_t read_from_mm_adr(const uint16_t mem_adr) { return 0xFF; }
void printf_flash(const char *fmt, ...) {}
void setup_interupt_timer(const uint16_t frequenzy_hz) {}
void init_measurement_ticks(void) {}
uint16_t get_tick_counter(void) { return 0xFF; }
void clear_tick_counter(void) {}

#else  // defined MOCK_HAL
/*
 * Hardware abstraction layer for Atmega 2560p
 * */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <stdio.h>

#include "context_switch_macros.hpp"

void write_mm(uint8_t *const mem_ptr, const uint8_t value) { *mem_ptr = value; }

uint8_t read_mm(uint8_t *const mem_ptr) { return *mem_ptr; }

uint8_t read_from_mm_adr(const uint16_t mem_adr) {
  return read_mm((uint8_t *)mem_adr);
}

void printf_flash(const char *fmt, ...) {
  const uint8_t sreg_at_entry = SREG;
  disable_interrupts();
  va_list args;
  va_start(args, fmt);
  vfprintf_P(stdout, fmt, args);
  va_end(args);
  SREG = sreg_at_entry;  // also restores interupt flag
}

NORETURN void halt_exec(void) {
  disable_interrupts();
  for (;;) {
  }
  __builtin_unreachable();
}

void setup_interupt_timer(__attribute__((unused)) const uint16_t frequenzy_hz) {
  constexpr uint16_t timer_compare_match =
      2000;  // FIXME: calcualte based on arument

  TCCR1B = (1 << CS10) | (1 << CS12) |
           (1 << WGM12);        // turn on CTC mode with 1024 presclarer
  TCNT1 = 0;                    // initialize counter value to 0
  OCR1A = timer_compare_match;  // set compare match register
}

void init_measurement_ticks(void) {
  TCCR3A = 0;  // set entire TCCR3A register to 0
  TCCR3B = 0;  // same for TCCR1B
  TCNT3 = 0;   // initialize counter value to 0
  // set compare match register to 10 Hz
  // Use normal mode.
  // TCCR2B |= (0 << WGM32);
  // Set CS32 for 256 prescaler
  TCCR3B |= 1 << CS30;
}

uint16_t get_tick_counter(void) { return TCNT3; }

void clear_tick_counter(void) {
  CLEAR_IO_REG(TCNT3H);
  CLEAR_IO_REG(TCNT3L);
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  CONTEXT_SWTICH();
  reti();
}

#endif  // defined MOCK_HAL
