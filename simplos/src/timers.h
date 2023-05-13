#ifndef TIMERS_H
#define TIMERS_H

#include <avr/interrupt.h>

#include "io_helpers.h"
// 781210 = (16*10^6) / (2*1024) - 1 (must be <65536) //FIXME
#define TIMER_COMPARE_MATCH 781  // aprox 100 ms

#define SCILENT_ENABLE_MT() TIMSK1 |= (1U << OCIE1A);

#define SCILENT_DISABLE_MT() TIMSK1 &= (uint8_t) ~(1U << OCIE1A);

#define ENABLE_MT()    \
  SCILENT_ENABLE_MT(); \
  cprint("enabling MT\n");

#define DISABLE_MT()   \
  SCILENT_DISABLE_MT() \
  cprint("disabling MT\n");

#define RESET_TIMER()   \
  CLEAR_IO_REG(TCNT1L); \
  CLEAR_IO_REG(TCNT1H);

__attribute__((noinline, naked)) void k_yield(void);
__attribute__((noinline)) uint16_t spawn_task(void (*fn)(void),
                                              uint8_t const priority,
                                              char const *name);
#if defined(SW_TIME_MEASSREMENTS)
// "return" the value of timer3.
#define GET_TICK_COUNTER() TCNT3

#define RESET_TICK_COUNTER() \
  CLEAR_IO_REG(TCNT3L);      \
  CLEAR_IO_REG(TCNT3H);
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
