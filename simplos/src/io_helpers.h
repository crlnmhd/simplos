#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdio.h>

#include "simplos_types.h"

// Disable clang warning
#if defined(__clang__)
// clang-format off
_Pragma("clang diagnostic push")
_Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
// clang-format on
#endif  // __clang__

#define print_from_prg_mem(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

#define HALT \
  for (;;)   \
    ;

#if defined(DEBUG_OUTPUT)
#define cprint(fmt, ...) print_from_prg_mem(fmt, ##__VA_ARGS__);
#else
#define cprint(fmt, ...) \
  { ; };
#endif

#if defined(__clang__)
    // clang-format off
_Pragma("clang diagnostic pop")
// clang-format on
#endif

#define ASSERT(cond, msg)                    \
  if (!(bool)(cond)) {                       \
    cli();                                   \
    print_from_prg_mem("ASSERTION ERROR! "); \
    print_from_prg_mem(msg);                 \
    print_from_prg_mem("\n");                \
    HALT                                     \
  }

#define ASSERT_EQ(recieved, expected, fmt, msg) \
  if ((expected) != (recieved)) {               \
    cli();                                      \
    print_from_prg_mem(                         \
        "ASSERT_EQUAL ERROR! "                  \
        "Expected: " fmt ", Got: " fmt "\n",    \
        expected, recieved);                    \
    print_from_prg_mem(msg "\n");               \
    HALT                                        \
  }

#define fatal_error(str, ...)             \
  cli();                                  \
  print_from_prg_mem("FATAL ERROR!\n");   \
  print_from_prg_mem(str, ##__VA_ARGS__); \
  print_from_prg_mem("\n");               \
  HALT

// Safe print from the os task
#define os_safe_print(fmt, ...)     \
  uint16_t old_sp = SP();           \
  SP = kernel->schedule.os_task_sp; \
  cprint(fmt, ##__VA_ARGS__);       \
  SP = old_sp;

#define warn(fmt, ...) print_from_prg_mem(fmt, ##__VA_ARGS__);

#if defined(HW_TIME_MEASSUREMENTS)
        static inline
    __attribute__((always_inline,
                   unused)) void output_curr_task(__attribute__((unused))
                                                  uint8_t task) {
  PORTB = task;
}
#endif
void fatal_error_internal(void);

void print_task(taskptr_type);
void print_schedule(void);

// void print_task_stack(uint8_t const pid);

// Print timing data (preferably after execution has finished).
void print_timing_data(void);

#endif  // IOHELPERS
