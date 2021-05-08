#include "interupts.h"

#include <stdio.h>

#include "io_helpers.h"

void init_timer_interupts(void) {
  cprint("Setting up interupts\n");
  // set timer1 interrupt at 2 Hz
  TCCR1A = 0;  // set entire TCCR1A register to 0
  TCCR1B = 0;  // same for TCCR1B
  TCNT1 = 0;   // initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = TIMER_COMPARE_MATCH;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
}
