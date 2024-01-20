#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <avr/pgmspace.h>
#include <stdbool.h>

#include "hal/hal.h"  // used by macros
#include "simplos_types.h"

void do_nothing_ignoreing_passed_parameters(__attribute__((unused))
                                            const char *fmt,
                                            ...);

#define print_from_prg_mem(fmt, ...) printf_flash(PSTR(fmt), ##__VA_ARGS__)

#if defined(DEBUG_OUTPUT)
#define debug_print(fmt, ...) print_from_prg_mem(fmt, ##__VA_ARGS__);
#else
#define debug_print(unused, ...) \
  do_nothing_ignoreing_passed_parameters(PSTR(unused), ##__VA_ARGS__)
#endif  // defined(DEBUG_OUTPUT)

#if defined(VERBOSE_OUTPUT)
#define verbose_print(fmt, ...) debug_print(fmt, ##__VA_ARGS__)
#else
#define ververbose_print(unused, ...) \
  do_nothing_ignoreing_passed_parameters(PSTR(unused), ##__VA_ARGS__)
#endif  // defined(VERBOSE_OUTPUT)

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
        (expected), (recieved));                \
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
// clang-format off

void print_task(taskptr_type, Kernel *kernel);
void print_schedule(Kernel *kernel);

// void print_task_stack(uint8_t const pid);

// Print timing data (preferably after execution has finished).
void print_timing_data(Kernel *kernel);

#endif  // IOHELPERS
