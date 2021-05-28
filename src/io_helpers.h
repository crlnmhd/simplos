#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <avr/pgmspace.h>
#include <stdio.h>

#include "simplos_types.h"

#define print_from_prg_mem(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

#ifdef DEBUG_OUTPUT
#define cprint(fmt, ...)                  \
  SCILENT_DISABLE_MT();                   \
  print_from_prg_mem(fmt, ##__VA_ARGS__); \
  SCILENT_ENABLE_MT();
#else
// #define dprint(...) ;
#define cprint(...) ;
#endif

void print_task(taskptr_t, bool);
#define fatal_error(str, ...)             \
  cli();                                  \
  print_from_prg_mem("FATAL ERROR!\n");   \
  print_from_prg_mem(str, ##__VA_ARGS__); \
  for (;;)                                \
    ;
void fatal_error_internal(void);

void print_schedule(void);

void print_task_stack(uint8_t const pid);

// Safe print from the os task
#define os_safe_print(fmt, ...) \
  uint16_t old_sp = *SP();      \
  SP = *os_task_sp;             \
  cprint(fmt, ##__VA_ARGS__);

#endif  // IO_HELPERS