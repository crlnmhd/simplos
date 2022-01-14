#include "timers.h"

#include <stdio.h>

#include "io_helpers.h"

// Use timer1 (16 bit) at 10 Hz context-swtich interupt generator.
void init_timer_interupts(void) {
  cprint("Setting up interupts\n");
  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   // initialize counter value to 0
  // set compare match register to 10 Hz
  OCR1A = TIMER_COMPARE_MATCH;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

// Use 16 bit timer to count approximate execution time.
void init_ticks(void) {
  cprint("Setting up ticks\n");
  TCCR3A = 0;  // set entire TCCR3A register to 0
  TCCR3B = 0;  // same for TCCR1B
  TCNT3 = 0;   // initialize counter value to 0
  // set compare match register to 10 Hz
  // Use normal mode.
  // TCCR2B |= (0 << WGM32);
  // Set CS32 for 256 prescaler
  TCCR2B |= 1 << CS30;
}

void print_cs_timing_data(void) { cprint("TODO: print timers"); }
