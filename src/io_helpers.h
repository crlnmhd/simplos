#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdio.h>

#include "simplos_types.h"

// Disable clang warning
#if defined(__clang__)
_Pragma("clang diagnostic push")
    _Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
#endif  // __clang__

#define print_from_prg_mem(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

#define HALT \
  for (;;)   \
    ;

#if defined(DEBUG_OUTPUT)
#define cprint(fmt, ...)                  \
  SCILENT_DISABLE_MT();                   \
  print_from_prg_mem(fmt, ##__VA_ARGS__); \
  SCILENT_ENABLE_MT();
#else
#endif

#if defined(__clang__)
        _Pragma("clang diagnostic pop")
#endif

#define ASSERT(cond, msg)                    \
  if (!(bool)(cond)) {                       \
    cli();                                   \
    print_from_prg_mem("ASSERTION ERROR! "); \
    print_from_prg_mem(msg);                 \
    print_from_prg_mem("\n");                \
    HALT                                     \
  }

#define ASSERT_EQ(expected, recieved, fmt, msg) \
  if ((expected) != (recieved)) {               \
    cli();                                      \
    print_from_prg_mem(                         \
        "ASSERT_EQUAL ERROR! "                  \
        "Expected: " fmt ", Got: " fmt "\n",    \
        expected, recieved);                    \
    print_from_prg_mem(msg);                    \
    HALT                                        \
  }

#define fatal_error(str, ...)             \
  cli();                                  \
  print_from_prg_mem("FATAL ERROR!\n");   \
  print_from_prg_mem(str, ##__VA_ARGS__); \
  HALT

// Safe print from the os task
#define os_safe_print(fmt, ...) \
  uint16_t old_sp = *SP();      \
  SP = *os_task_sp;             \
  cprint(fmt, ##__VA_ARGS__);

#if defined(HW_TIME_MEASSUREMENTS)
            static inline
    __attribute__((always_inline,
                   unused)) void output_curr_task(__attribute__((unused))
                                                  uint8_t task) {
  PORTB = task;
}
#endif
void fatal_error_internal(void);

void print_task(taskptr_t, bool);
void print_schedule(void);

// void print_task_stack(uint8_t const pid);

// Print timing data (preferably after execution has finished).
void print_timing_data(void);

#endif  // IO_HELPERS
