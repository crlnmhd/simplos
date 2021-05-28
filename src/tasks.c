#include "tasks.h"

#include <stdio.h>

#include "io_helpers.h"
#include "os.h"

// extern
volatile uint8_t shared_x = 5;

#if 1
void sum_to_ten(void) {
  uint8_t res = 0;
  for (uint8_t i = 0; i < 10; ++i) {
    res += i;
  }
  for (uint16_t i = 0; i < UINT16_MAX; ++i) {
    // do nothing
    ;
  }
  // tmp
  shared_x += res;
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
  pid_t const p = spawn(wait_for_me, 1);

  print("Waiting for other task to finnish\n");
  wait_for_task_finnish(p);
  print("It's finnished!\n");
}

void idle_fn(void) {
  shared_x = 0;
  print("Starting idle function\n");
  pid_t p1 = spawn(sum_to_ten, 1);
  pid_t p2 = spawn(sum_to_ten, 1);
  while (shared_x != 45 * 2) {
    ;
  }
  print("pids are p1: %d and p2 : %d. My PID is : %d \n", p1, p2, pid());
  print("Heavy duty shared memory calculations performed!\n");

  wait_for_other();

  terminate();
}
#else

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

void set_shared_to(uint8_t const val) { shared_x = val; }

void test_yield(void) {
  for (uint8_t i = 0; i < 20; ++i) {
    print("$$$$  %d yielding $$$\n", i);
    // print("Hello! I'm not supposed to tell you this, but my sp is 0x%X\n",
    //       *task_sp);

    yield();

    // print("The sp is 0x%X. Oops I did it again.\n", *task_sp);
    print("hi there!\n");
    ++shared_x;

    print("%d is a number\n", shared_x);
    print("%d --- reincarnated\n", i);
  }
  print("All done!\n");
}

void test_context_switch(void) {
  uint8_t v = 0;
  print("Doing realy long work") for (uint16_t i = 0; i < 10; ++i) {
    for (uint16_t j = 0; j < 500; ++j) {
      print("@");
      ++v;
    }
    print("\n%d\n", i);
  }
  print("I survived the constext switch test!!\n");
}

void test_set_to_10(void) {
  print("in test_set_to_10\n");
  shared_x = 10;
}

void test_fn5(void) {
  print("fn 5\n");
  print("x = %d\n", ++shared_x);
  // for (;;)
  //   ;
  // yield();
  print("hi there!\n");
}

void test_fn_x_5_to_50(void) {
  while (shared_x != 5) {
    ;
  }
  shared_x = 50;
}

void test_fn_x_to_5(void) {
  for (uint16_t i = 0; i < UINT16_MAX; ++i) {
    ;
  }
  shared_x = 5;
}

void wait_test(void) {
  shared_x = 123;
  for (int i = 0; i < 255; ++i) {
    ;
  }
  shared_x = 255;
}

void tests(void) {
  print("Trying waiting\n");
  // test_context_switch();

  print(
      "Beginning of tests\nTest 1: Testing stack integrity by calling an "
      "ordinary "
      "function.\n");
  int const expted = 123;
  set_shared_to(expted);
  if (shared_x != expted) {
    print("Error! Test 1 failed. Ordinary function call did not work!");
    HALT_EXEC();
  }
  shared_x = 0;
  print("Test 2: Spawning a task that sets shared_x \n");
  print_schedule();
  print("Schedule printed\n");
  spawn(test_set_to_10, 4);
  print("After: %d\n", shared_x);
  if (shared_x != 10) {
    print("Error! Test 2 failed. Spawning did not work");
    HALT_EXEC();
  }

  // Test creating many tasks that are quickly killed.
  print("Test 3 Testing spawning multiple task that die instantly\n");

  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  spawn(test_fn1, 1);
  print("First spawn test PASSED\n");

  print("Testing shared variables\n");
  shared_x = 10;
  spawn(test_fn_x_5_to_50, 1);
  spawn(test_fn_x_to_5, 1);
  while (shared_x != 50) {
    ;
  }
  print("Tests passed!");
}

void idle_fn() {
  // print("In idle loop. Running tests.\n");
  // tests();
  shared_x = 41;
  // print("In idle loop. shared x = %d\n", shared_x);
  // print("Testing context switches\n");
  // test_context_switch();
  // print("Test of context switch passed!\n");

  // print("Testing yields\n");
  // test_yield();

  // spawn(test_fn5, 2);
  // for (;;) {
  //   print("Idle task yielding\n");
  //   yield();
  // };
  print("Running tests\n");
  tests();

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

#endif