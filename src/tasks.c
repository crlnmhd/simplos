#include "tasks.h"

#include <stdio.h>

#include "simplos.h"

// extern
volatile uint8_t shared_x = 5;

void test_fn1(void) {
  DISABLE_MT();
  printf("Running fn1\n");
  ENABLE_MT();

  shared_x = 10;
}

void test_fn2(void) {
  printf("fn2\n");
  for (;;) {
    DISABLE_MT();
    printf("2:: x = %d\n", shared_x);
    DISABLE_MT();
  }
}

void test_fn3(void) {
  for (;;) {
    printf("3 touch and go!\n");
    ++shared_x;
    yield();
  }
}

void idle_fn(volatile Scheduler* schedule) {
  printf("In idle loop!\n");
  ENABLE_MT();
  // main_sp = *STACK_POINTER;
  // printf("Starting task 1\n");
  // SPAWN_TASK(test_fn1, 1, schedule);

  printf("starting task 2\n");
  SPAWN_TASK(test_fn2, 1, schedule);

  printf("Starting task 3\n");
  SPAWN_TASK(test_fn3, 1, schedule);

  for (;;) {
    printf("Yielding from idle loop!\n");
    yield();
  }

  // Finally let the scheduler run!
  for (;;) {
    for (uint16_t i = 0; i < 0xFFFF; ++i) {
      ;
    }
    printf("i");
  }
}