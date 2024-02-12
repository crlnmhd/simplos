#ifndef TIMERS_H
#define TIMERS_H

#include "hal/context_switch_macros.hpp"
#include "inline.hpp"
#include "io_helpers.hpp"

INLINED void enable_mt() {
  SCILENT_ENABLE_MT();
  debug_print("enabling MT\n");
}

INLINED void disable_mt() {
  scilent_disable_mt();
  debug_print("disabling MT\n");
}

__attribute__((noinline, naked)) void k_yield(void);
__attribute__((noinline)) uint16_t spawn_task(void (*fn)(void),
                                              uint8_t const priority,
                                              const ProgmemString &name,
                                              Kernel *kernel);
#if defined(SW_TIME_MEASSREMENTS)
uint16_t get_tick_counter(void){return get_tick_counter()};
void reset_tick_counter(void) { clear_tick_counter(); }
#endif  // SW_TIME_MEASSREMENTS

void init_scheduler_interupts(void);

#if defined(SW_TIME_MEASSREMENTS)
void init_ticks(void);  // Enable timer ticks.
void print_cs_timing_data(void);
#endif  // SW_TIME_MEASSREMENTS

#endif  // TIMERS_H
