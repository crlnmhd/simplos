#ifndef TIMERS_H
#define TIMERS_H

#include <avr/interrupt.h>

#include "io_helpers.h"
// 781210 = (16*10^6) / (2*1024) - 1 (must be <65536) //FIXME
#define TIMER_COMPARE_MATCH 781  // aprox 100 ms

#define SCILENT_ENABLE_MT()                                             \
  asm volatile(                                                         \
      "push r16                         \n\t "                          \
      "lds r16, %[timer_adr]            \n\t "                          \
      "ori r16, (1 << %[enable_bit])    \n\t "                          \
      "sts %[timer_adr], r16            \n\t "                          \
      "pop r16                          \n\t " ::[timer_adr] "i"(0x6F), \
      [enable_bit] "I"(OCIE1A));  // Set enable bit for TIMSK1

#define SCILENT_DISABLE_MT()                                            \
  asm volatile(                                                         \
      "push r16                         \n\t "                          \
      "lds r16, %[timer_adr]            \n\t "                          \
      "andi r16, ~(1 << %[enable_bit])  \n\t "                          \
      "sts %[timer_adr], r16            \n\t "                          \
      "pop r16                          \n\t " ::[timer_adr] "i"(0x6F), \
      [enable_bit] "I"(OCIE1A));  // Unset enable bit for TMSK1

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
uint16_t get_tick_counter(void){return get_tick_counter()};

#define RESET_TICK_COUNTER() clear_tick_counter()

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
