#ifndef INTERUPTS_H_
#define INTERUPTS_H_

#include <avr/interrupt.h>

// I known, I know
#define NO_MT(code) \
  DISABLE_MT();     \
  code;             \
  ENABLE_MT();

#define SCILENT_ENABLE_MT() \
  TIMSK1 |= (1 << OCIE2A);  \
  sei();

#define SCILENT_DISABLE_MT() TIMSK1 &= ~(1 << OCIE2A);

#define ENABLE_MT()    \
  SCILENT_ENABLE_MT(); \
  printf("enabling MT\n");

#define DISABLE_MT()   \
  SCILENT_DISABLE_MT() \
  printf("disabling MT\n");

void init_timer_interupts(void);

#endif  // INTERUPTS_H_