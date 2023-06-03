#include "tasks.h"

#include <stdio.h>

#include "io_helpers.h"
#include "os.h"
#include "scheduler.h"
#include "tests/test.h"

// extern
// volatile uint8_t shared_x = 5;

void worker_1_fn(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("worker 1: %d%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX; ++j) {
      ;
    }
  }
}

void worker_2_fn(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("worker 2: %d%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX; ++j) {
      ;
    }
  }
}

void sum_to_ten(void) {
  uint8_t res = 0;
  for (uint8_t i = 1; i < 10; ++i) {
    res = (uint8_t)(res + i);
    yield();
    yield();
  }
  for (uint16_t i = 0; i < UINT16_MAX; ++i) {
    // do nothing
    ;
  }
  print("res from sum is:%d\n", res);
  // print("shared_x is:%d\n", shared_x);
  // shared_x += res;
}

void wait_for_me(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print(".\n");
    for (uint16_t j = 0; j < UINT16_MAX; ++j) {
      ;
    }
  }

  print("I'm done!\n");
}

void wait_for_other(void) {
  print("Spawning worker function\n");
  pid_t const p = spawn(wait_for_me, 1, "waiter");

  print("Waiting for task with pid %d to finnish\n", p);
  wait_for_task_finnish(p);
  print("It's finnished!\n");
}

void idle_fn(void) {
  print("Starting scheduler\n");
  spawn(start_scheduler, 0, "OS sched");  // yields emidiatly.
  print("Running static tests\n");
  run_static_tests();
  print("Starting idle function\n");

#if defined(RUN_TESTS)
  run_tests();
#endif  // defined(RUN_TESTS)

  print("Testing context switch ratio\n");
  spawn(worker_1_fn, 1, "worker1");
  spawn(worker_2_fn, 1, "worker2");

  // shared_x = 1;
  pid_t const p1 = spawn(sum_to_ten, 1, "sto10_1");
  pid_t const p2 = spawn(sum_to_ten, 1, "sto10_2");

  print("Sum to ten workers %d and %d started\n", p1, p2);

  print("Hi. Fuck Putin\n");
  // print("Adr of shared_x 0x%X\n", &shared_x);
  print("SP is : 0x%X\n", SP);
  // print("Val of shared_x 0x%X\n", shared_x);
  print("SP is : 0x%X\n", SP);
  // while (shared_x != 45 * 2) {
  //   print("shared x:%d\n", shared_x);
  //   ;
  // }
  print("Voff voff mjau mjau\n");
  // print("pids are p1: %d and p2 : %d. My PID is : %d \n", p1, p2, pid());
  print("Heavy duty shared memory calculations performed!\n");

  wait_for_other();

  terminate();
}
