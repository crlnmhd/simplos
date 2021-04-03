#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>

#include "simplos_types.h"

/***********     Shared memory ***********/

// TODO mutex / semaphore.
// Atomic access will do for now.

extern volatile uint8_t shared_x;

/*********** End of shared memory ***********/

void test_fn1(void);
void test_fn2(void);
void test_fn3(void);

void idle_fn(volatile Scheduler* schedule);

#endif