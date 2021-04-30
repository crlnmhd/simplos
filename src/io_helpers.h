#ifndef IO_HELPERS_
#define IO_HELPERS_

#include <stdio.h>

#include "simplos_types.h"

#ifdef DEBUG_OUTPUT
#define dprint(...)     \
  SCILENT_DISABLE_MT(); \
  printf(__VA_ARGS__);  \
  SCILENT_ENABLE_MT();
#else
#define dprint(...) ;
#endif

void print_task(Simplos_Task *, bool);
void fatal_error(char const *fmt, ...);

void print_schedule(Scheduler *);

#endif  // IO_HELPERS