#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>


/***********     Shared memory ***********/

// TODO mutex / semaphore.
// Atomic access will do for now.

static volatile uint8_t shared_x = 5;

/*********** End of shared memory ***********/

void test_fn1(void);
void test_fn2(void);
void test_fn3(void);

#endif 