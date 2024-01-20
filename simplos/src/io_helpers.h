#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <avr/pgmspace.h>
#include <stdbool.h>

#include "hal/hal.h"  // used by macros
#include "simplos_types.h"

// Disable clang warning
#if defined(__clang__)
// clang-format off
_Pragma("clang diagnostic push")
_Pragma("clang diagnostic ignored \"-Wunknown-attributes\"")
// clang-format on
#endif  // __clang__

#define print_from_prg_mem(fmt, ...) printf_flash(PSTR(fmt), ##__VA_ARGS__)

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
    disable_interrupts();                    \
    print_from_prg_mem("ASSERTION ERROR! "); \
    print_from_prg_mem(msg);                 \
    print_from_prg_mem("\n");                \
    halt_exec();                             \
  }

#define ASSERT_EQ(recieved, expected, fmt, msg) \
  if ((expected) != (recieved)) {               \
    disable_interrupts();                       \
    print_from_prg_mem(                         \
        "ASSERT_EQUAL ERROR! "                  \
        "Expected: " fmt ", Got: " fmt "\n",    \
        expected, recieved);                    \
    print_from_prg_mem(msg "\n");               \
    halt_exec();                                \
  }

#define FATAL_ERROR(str, ...)             \
  disable_interrupts();                   \
  print_from_prg_mem("FATAL ERROR!\n");   \
  print_from_prg_mem(str, ##__VA_ARGS__); \
  print_from_prg_mem("\n");               \
  halt_exec();

#define WARNING(fmt, ...)          \
  print_from_prg_mem("WARNING: "); \
  print_from_prg_mem(fmt, ##__VA_ARGS__);

        void print_task(taskptr_type);
void print_schedule(void);

// void print_task_stack(uint8_t const pid);

// Print timing data (preferably after execution has finished).
void print_timing_data(void);

#endif  // IOHELPERS
