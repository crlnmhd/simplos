#include "tasks.h"

#include <stdio.h>

#include "simplos.h"

// extern
volatile uint8_t shared_x = 5;

void test_fn1(void) {
  printf("Running fn1\n");

  shared_x = 11;
}

void test_fn2(void) {
  printf("fn2\n");
  for (;;) {
    printf("2:: x = %d\n", shared_x);
    // yield();
  }
}

void test_fn3(void) {
  for (;;) {
    printf("3 touch and go!\n");
    ++shared_x;
    yield();
  }
}

void test_fn4(void) {
  for (;;) {
    for (int i = 0; i < 10; ++i) {
      printf("fn4: HERE I AM!\n");
    }
    ++shared_x;
  }
}

void idle_fn(volatile Scheduler* schedule) {
  // printf("old sp %u\n", sp);
  shared_x = 42;

  dprint("In idle loop. shared x = %d\n", shared_x);
  ENABLE_MT();
  print_schedule(schedule);

  printf("Starting task 1\n");
  spawn_task(test_fn1, 1, schedule);

  dprint("idle fn starting task 2\n");
  spawn_task(test_fn2, 1, schedule);

  dprint("idle fn starting task 3\n");
  spawn_task(test_fn3, 1, schedule);

  dprint("idle fn starting task 4\n");
  spawn_task(test_fn4, 1, schedule);

  // for (;;) {
  //   printf("Idle fn goes round and round\n");
  // }

  dprint("Idle fn killing itself\n");

  kill_current_task(schedule);
  fatal_error("UNREACHABLE END OF IDLE LOOP");

  // for (;;) {
  //   printf("Yielding from idle loop!\n");
  //   yield();
  // }

  // // Finally let the scheduler run!
  // for (;;) {
  //   for (uint16_t i = 0; i < 0xFFFF; ++i) {
  //     ;
  //   }
  //   printf("i");
  // }
}