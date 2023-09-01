#ifndef TIMERS_H
#define TIMERS_H

#include <avr/interrupt.h>

#include "io_helpers.h"

#define ENABLE_MT()    \
  SCILENT_ENABLE_MT(); \
  cprint("enabling MT\n");

#define DISABLE_MT()   \
  SCILENT_DISABLE_MT() \
  cprint("disabling MT\n");

__attribute__((noinline, naked)) void k_yield(void);
__attribute__((noinline)) uint16_t spawn_task(void (*fn)(void),
                                              uint8_t const priority,
                                              char const *name);
#if defined(SW_TIME_MEASSREMENTS)

uint16_t get_tick_counter(void){return get_tick_counter()};

void reset_tick_counter(void) { clear_tick_counter(); }

#endif  // SW_TIME_MEASSREMENTS

void init_scheduler_interupts(void);

void TIMER1_COMPA_vect(void);

#if defined(SW_TIME_MEASSREMENTS)
/*
  Enable timer ticks.
*/
void init_ticks(void);
void print_cs_timing_data(void);
#endif  // SW_TIME_MEASSREMENTS

#endif  // TIMERS_H
