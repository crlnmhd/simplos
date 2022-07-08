#if defined(RUN_TESTS)
#include "test.h"

#include <avr/pgmspace.h>

#include "os.h"
#define SKIP_TEST(test_fn, function_name, test_statistics)       \
  test_statistics.skipped += 1;                                  \
  /* This is the least of the security concerns, so let it be.*/ \
  printf_P(PSTR(function_name));

#define print_from_prg_mem(fmt, ...) printf_P(PSTR(fmt), ##__VA_ARGS__)

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       struct TestStatistics statistics) {
  print_from_prg_mem("Testing: ");
  printf_P(function_name);
  print_from_prg_mem("\n");
  if (!fn_ptr()) {
    statistics.failed += 1;
    printf_P(function_name);  // In an 'OS' with so many security concerns, this
                              // is the least of our problems.
    print_from_prg_mem(" failed");
  } else {
    statistics.passed += 1;
  }
}

#define TEST_ASSERT(cond, msg)                \
  if (!(bool)(cond)) {                        \
    asm volatile("cli");                      \
    print_from_prg_mem(" FAILED!\n");         \
    print_from_prg_mem("ASSERTION FAILED! "); \
    print_from_prg_mem(msg);                  \
    print_from_prg_mem("\n");                 \
    asm volatile("sei");                      \
    return false;                             \
  }

#define TEST_ASSERT_EQ(recieved, expected, fmt, msg) \
  if ((expected) != (recieved)) {                    \
    asm volatile("cli");                             \
    print_from_prg_mem(" FAILED\n");                 \
    print_from_prg_mem(                              \
        "ASSERT_EQUAL ERROR! "                       \
        "Expected: " fmt ", Got: " fmt "\n",         \
        expected, recieved);                         \
    print_from_prg_mem("%s\n", msg);                 \
    asm volatile("sei");                             \
    return false;                                    \
  }

// Wrapper to put 'function_name' string in program memory.
#define RUN_TEST(fn, function_name, test_statistics) \
  run_test_function(fn, PSTR(function_name), test_statistics);

bool test_single_yeild(void) {
  int x = 42;
  yield();
  TEST_ASSERT_EQ(x, 42, "%d", "Yield caused funny issues");
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
  for (uint8_t i = 0; i < (uint8_t)UINT8_MAX; ++i) {
    yield();
    test_var += (uint32_t)(i * i);  // perform some non_atomic calculations
    yield();
  }
  TEST_ASSERT_EQ(test_var, 5494655, "%d",
                 "Numerical issue with aggressive task yielding");
  return true;
}

struct TestStatistics run_all_tests(void) {
  struct TestStatistics test_statistics = {
      .passed = 0, .failed = 0, .skipped = 0};

  RUN_TEST(test_single_yeild, "test single yield does not crash the OS",
           test_statistics);
  RUN_TEST(test_multiple_yield, "test multiple yield does not crash",
           test_statistics);

  return test_statistics;
}

void run_tests(void) {
  print("Running tests\n");
  struct TestStatistics test_stats = run_all_tests();
  print("\n");
  print("%d tests PASSED\n", test_stats.passed);
  print("%d tests FAILED\n", test_stats.failed);
  print("%d tests SKIPPED \n", test_stats.skipped);
}

#endif  // defined(RUN_TESTS)
