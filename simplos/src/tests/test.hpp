#if !defined(TESTS_H_)
#define TESTS_H_
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>

#include "../hal/log.hpp"

#if !defined(MOCK_HAL) && \
    !defined(__clang__)  // FIXME: just define separate run mode
#error \
    "Test header can not be included in non-test mode due to memory contraints."
#endif  // MOCK_HAL

#include "../io_helpers.hpp"

extern Log hal_log;

#define TEST_PASSED true

struct TestStatistics {
  TestStatistics() : failed(0), passed(0), skipped(0) {}

  uint8_t failed;
  uint8_t passed;
  uint8_t skipped;
};

void combine_statistics(TestStatistics &statistics, TestStatistics &to_add);

#define SKIP_TEST(test_fn, function_name, test_statistics)       \
  test_statistics.skipped += 1;                                  \
  /* This is the least of the security concerns, so let it be.*/ \
  printf_P(PSTR(function_name));

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       TestStatistics &statistics);

#define CHECK_EQ(recieved, expected, fmt)              \
  if ((expected) != (recieved)) {                      \
    debug_printf(                                      \
        "\n%S at line: %d:\nEquality check failed! "   \
        "Expected: " fmt ", Got: " fmt "\n",           \
        PSTR(__FILE__), __LINE__, expected, recieved); \
    return false;                                      \
  }
#define CHECK_TRUE(recieved)                                           \
  if ((recieved) == false) {                                           \
    debug_printf(                                                      \
        "\n%S line %d:\nCheck failed! Expected 'true' got 'false'!\n", \
        PSTR(__FILE__), __LINE__);                                     \
    return false;                                                      \
  }

#define CHECK_FALSE(recieved)                                          \
  if ((recieved) == true) {                                            \
    debug_printf(                                                      \
        "\n%S line %d:\nCheck failed! Expected 'false' got 'true'!\n", \
        PSTR(__FILE__), __LINE__);                                     \
    return false;                                                      \
  }
// Wrapper to put 'function_name' string in program memory.
#define RUN_TEST(fn, test_statistics) \
  run_test_function(fn, PSTR(#fn), test_statistics);

#define debug_printf(fmt, ...) debug_printf_flash(PSTR(fmt), ##__VA_ARGS__)

void debug_printf_flash(const char *fmt, ...);
#endif  // defined(TESTS_H_)
