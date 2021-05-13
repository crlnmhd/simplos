#include "tasks.h"

#include <stdio.h>

#include "os.h"

// extern
volatile uint8_t shared_x = 5;

void test_fn1(void) {
  print("Running fn1\n");

  shared_x = 11;
  // yield();
}

void test_fn2(void) {
  print("fn2\n");
  for (;;) {
    print("2:: x = %d\n", shared_x);
    // yield();
  }
}

void test_fn3(void) {
  for (;;) {
    print("3 touch and go!\n");
    ++shared_x;
    yield();
  }
}

void test_fn4(void) {
  for (;;) {
    for (int i = 0; i < 10; ++i) {
      print("fn4: HERE I AM!\n");
    }
    ++shared_x;
  }
}

void test_context_switch(void) {
  SAVE_SP();
  SAVE_SP();
  for (uint8_t i = 0; i < 20; ++i) {
    print("$$$$  %d yielding $$$\n", i);
    SAVE_SP();
    print("Hello! I'm not supposed to tell you this, but my sp is 0x%X\n",
          *task_sp);
    yield();
    SAVE_SP();
    print("The sp is 0x%X. Oops I did it again.\n", *task_sp);
    print("hi there!\n");
    ++shared_x;

    print("%d is a number\n", shared_x);
    print("%d --- reincarnated\n", i);
  }
  print("All done!\n");
}

void test_fn5(void) {
  print("fn 5\n");
  print("x = %d\n", ++shared_x);
  // for (;;)
  //   ;
  yield();
  print("hi there!\n");
}

void idle_fn() {
  shared_x = 41;

  print("In idle loop. shared x = %d\n", shared_x);

  test_context_switch();
  print("Test of context switch passed!\n");

  // spawn(test_fn5, 2);
  // for (;;) {
  //   print("Idle task yielding\n");
  //   yield();
  // };

  print("Starting task 1\n");
  spawn(test_fn1, 1);

  print("idle fn starting task 2\n");
  spawn(test_fn2, 1);

  print("r0 I (task %d) RETURNED FROM SPAWNING TASK 2 !!!!!\n", pid());
  print("r1 I (task %d) RETURNED FROM SPAWNING TASK 2 !!!!!\n", pid());
  print("r2 I (task %d) RETURNED FROM SPAWNING TASK 2 !!!!!\n", pid());
  print("r3 I (task %d) RETURNED FROM SPAWNING TASK 2 !!!!!\n", pid());
  print("r4 I (task %d) RETURNED FROM SPAWNING TASK 2 !!!!!\n", pid());

  print("idle fn starting task 3\n");
  spawn(test_fn3, 1);

  print("idle fn starting task 4\n");
  spawn(test_fn4, 1);

  print("Idle fn killing itself\n");

  kill_curr_task();
  terminate();
}