#include "../hal.h"
#include "../io_helpers.h"
#include "../os.h"
#include "../timers.h"
#include "test.h"

static bool test_single_yeild(void);
static bool test_multiple_yield(void);
static bool test_long_running_function(void);

void run_tests(void) {
  print("Running tests\n");
  struct TestStatistics test_stats = run_all_tests();
  print("\n");
  print("%d tests PASSED\n", test_stats.passed);
  print("%d tests FAILED\n", test_stats.failed);
  print("%d tests SKIPPED \n", test_stats.skipped);
}

bool test_single_yeild(void) {
  print(" in fn task sp: 0x%X\n", SP);

  volatile uint16_t x = 42;
  print("x = %d\n", x);
  print("x lives here: 0x%X\n", &x);
  print("x = %d\n", x);
  asm volatile("push r16");
  asm volatile("ldi r16, 0xFF");
  asm volatile("push r16");
  asm volatile("push r16");
  asm volatile("push r16");
  yield();
  TEST_ASSERT_EQ(x, 42, "%d", "Yield caused funny issues");
  print("IT ALL WORKED!\n");
  return true;
}

bool test_double_yeild(void) {
  int a = 41;
  yield();
  a++;
  yield();
  TEST_ASSERT_EQ(a, 42, "%d", "Double yield caused funny issues");

  int b = 41;
  {
    yield();
    b++;
    yield();
  }
  uint16_t c = 41;
  {
    yield();
    c++;
    yield();
  }
  TEST_ASSERT_EQ(c, 42, "%d", "Double yield in scope caused funny issues");
  return true;
}

bool test_multiple_yield(void) {
  print("Agressivly yielding the task\n");
  yield();
  yield();
  yield();
  yield();
  yield();
  print("And now in a loop\n");
  uint32_t test_var = 0;
  // 60 iterations is enough to overflow a uint16_t
  for (uint8_t i = 0; i < 60; i++) {
    print("iteration %d\n", i);
    yield();
    test_var +=
        (uint32_t)i * (uint32_t)i;  // perform some non_atomic calculations
    yield();
  }
  TEST_ASSERT_EQ(test_var, 70210, "%d",
                 "Numerical issue with aggressive task yielding");
  return true;
}

bool test_long_running_function(void) {
  for (uint16_t i = 0; i < 100; i++) {
    // printf("Tupp\n");
    print("Tuff\n");
  }
  return true;
}

void test_fn1(void) { test_single_yeild(); }

struct TestStatistics run_all_tests(void) {
  struct TestStatistics test_statistics = {
      .passed = 0, .failed = 0, .skipped = 0};

  // pid_t pid = spawn(test_fn1, 1, "yield t");
  // wait_for_task_finnish(pid);

  RUN_TEST(test_long_running_function,
           "test that a long running function can work for a while.",
           test_statistics);
  RUN_TEST(test_single_yeild, "test single yield does not crash the OS",
           test_statistics);
  RUN_TEST(test_double_yeild, "test double yield does not crash the OS",
           test_statistics);
  RUN_TEST(test_multiple_yield, "test multiple yield does not crash",
           test_statistics);

  return test_statistics;
}
