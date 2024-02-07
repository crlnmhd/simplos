#include <stdio.h>

#include "../io_helpers.h"
#include "../os.h"
#include "../scheduler.h"
#include "idle_function.h"

volatile uint8_t shared_x = 5;

void just_a_loop(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("looper 1: %u%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX / 32; ++j) {
      ;
    }
  }
  print("Loop DONE\n");
}

void worker_1_fn(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("worker 1: %u%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX / 4; ++j) {
      ;
    }
  }
  print("1 DONE\n");
}

void worker_2_fn(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("worker 2: %u%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX / 4; ++j) {
      ;
    }
  }
  print("2 DONE\n");
}

void do_some_yields(void) {
  for (uint16_t i = 0; i < 5; ++i) {
    print("yielder yielding\n");
    yield();
  }

  print("I'm done!\n");
}

void wait_for_child(void) {
  for (uint16_t i = 0; i < 100; ++i) {
    print("waitin a bit : %u%%\n", i);
    for (uint16_t j = 0; j < UINT16_MAX / 4; ++j) {
      ;
    }
  }
  print("Spawning child task\n");
  // const uint16_t waiter_pid = spawn(do_some_yields, 1, "yielder");
  // wait_for_task_finnish(waiter_pid);
}

void run_idle_fn(void) {
#ifdef MOCK_HAL
  FATAL_ERROR("Can not run idle function on mock hal")
#else

  print("Starting scheduler\n");
  spawn(start_scheduler_with_os_kernel, 0, "OS sched");  // yields emidiatly.

  print("Startin single task\n");
  const uint16_t singel_task_pid = spawn(just_a_loop, 0, "looper");

  print("Waiting for task to finnish\n");
  wait_for_task_finnish(singel_task_pid);

  spawn(worker_2_fn, 1, "worker2");
  const uint16_t wait_for_child_pid = spawn(wait_for_child, 2, "waits");
  const uint16_t worker1_pid = spawn(worker_1_fn, 1, "worker1");
  wait_for_task_finnish(wait_for_child_pid);

  wait_for_task_finnish(worker1_pid);
  // wait_for_task_finnish(worker2_pid);

  terminate();
#endif  // MOCK_HAL
}
