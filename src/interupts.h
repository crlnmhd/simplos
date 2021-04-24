#ifndef INTERUPTS_H_
#define INTERUPTS_H_

#include <avr/interrupt.h>

#include "io_helpers.h"

#define TIMER_COMPARE_MATCH \
  7812;  // = (16*10^6) / (2*1024) - 1 (must be <65536) //FIXME

#define SCILENT_ENABLE_MT() TIMSK1 |= (1 << OCIE2A);

#define SCILENT_DISABLE_MT() TIMSK1 &= ~(1 << OCIE2A);

#define ENABLE_MT()    \
  SCILENT_ENABLE_MT(); \
  dprint("enabling MT\n");

#define DISABLE_MT()   \
  SCILENT_DISABLE_MT() \
  dprint("disabling MT\n");

// I known, I know
#define NO_MT(code) \
  DISABLE_MT();     \
  code;             \
  ENABLE_MT();

void init_timer_interupts(void);

#endif  // INTERUPTS_H_