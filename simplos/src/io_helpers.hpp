#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <stdbool.h>

#include "hal/hal.hpp"  // used by macros
#include "simplos_types.hpp"

void do_nothing_ignoreing_passed_parameters(__attribute__((unused))
                                            const char *fmt,
                                            ...);

// Workaround due to avr-libc issue #898
// https://github.com/avrdudes/avr-libc/issues/898
#undef FDEV_SETUP_STREAM
#define FDEV_SETUP_STREAM(PU, GE, FL)                                      \
  {                                                                        \
    0 /* buf */, 0 /* unget */, FL /* flags */, 0 /* size */, 0 /* len */, \
        PU /* put */, GE /* get */, 0 /* udata */                          \
  }

#define print_from_prg_mem(fmt, ...) printf_flash(PSTR(fmt), ##__VA_ARGS__)

// void print(const uint8_t u8) { print_from_prg_mem("%u", u8); }
// void print(const uint16_t u16) { print_from_prg_mem("%ul", u16); }

#if defined(DEBUG_OUTPUT)
#define debug_print(fmt, ...) print_from_prg_mem(fmt, ##__VA_ARGS__);
#else
#define debug_print(unused, ...) \
  do_nothing_ignoreing_passed_parameters(PSTR(unused), ##__VA_ARGS__)
#endif  // defined(DEBUG_OUTPUT)

#if defined(VERBOSE_OUTPUT)
#define verbose_print(fmt, ...) debug_print(fmt, ##__VA_ARGS__)
#else
#define verbose_print(unused, ...) \
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

void print_task(Simplos_Task *, Kernel *kernel);
void print_schedule(Kernel *kernel);

// void print_task_stack(uint8_t const pid);

// Print timing data (preferably after execution has finished).
void print_timing_data(Kernel *kernel);

#endif  // IOHELPERS
