#ifndef TIMERS_H
#define TIMERS_H

#include <avr/interrupt.h>

#include "io_helpers.h"
// 781210 = (16*10^6) / (2*1024) - 1 (must be <65536) //FIXME
#define TIMER_COMPARE_MATCH 781  // aprox 100 ms

#define SCILENT_ENABLE_MT() TIMSK1 |= (1 << OCIE1A);

#define SCILENT_DISABLE_MT() TIMSK1 &= ~(1 << OCIE1A);

#define ENABLE_MT()    \
  SCILENT_ENABLE_MT(); \
  cprint("enabling MT\n");

#define DISABLE_MT()   \
  SCILENT_DISABLE_MT() \
  cprint("disabling MT\n");

#if defined(SW_TIME_MEASSREMENTS)
// "return" the value of timer3.
#define GET_TICK_COUNTER() TCNT3

#define RESET_TICK_COUNTER() TCNT3 = 0;
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
