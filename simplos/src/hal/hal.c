#include "../hal.h"

#ifdef TEST_HAL
HardwareState state = {.interrupt_enabled = false};
void halt_exec(void) { ; }
void enable_interrupts(void) { state.interrupt_enabled = true; }
void disable_interrupts(void) { state.interrupt_enabled = false; }
#else  // TEST_HAL

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

void setup_interupt_timer(__attribute__((unused)) const uint16_t frequenzy_hz) {
  const uint16_t timer_compare_match =
      781;  // FIXME: calcualte based on arument

  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   // initialize counter value to 0
  // set compare match register to 10 Hz
  OCR1A = timer_compare_match;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
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
  CLEAR_IO_REG(TCNT3L);
  CLEAR_IO_REG(TCNT3H);
}

ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  CONTEXT_SWTICH();
  reti();
}
#endif  // TEST_HAL
