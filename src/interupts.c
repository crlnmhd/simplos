#include "interupts.h"

#include <stdio.h>

#include "io_helpers.h"


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

TODO: thins
void init_ticks(int const frequency_kHz){
  cprint("Setting up ticks\n");
  TCCR2A = 0;  // set entire TCCR1A register to 0
  TCCR2B = 0;  // same for TCCR1B
  TCNT2 = 0;   // initialize counter value to 0
  // set compare match register to 10 Hz
  OCR2A = 71 * frequency_kHz;
  // turn on CTC mode
  TCCR2B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR2B |= (1 << CS12) | (1 << CS10);
}

