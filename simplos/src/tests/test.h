#if !defined(TESTS_H_)
#define TESTS_H_
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>

#if !defined(MOCK_HAL) && \
    !defined(__clang__)  // FIXME: just define separate run mode
#error \
    "Test header can not be included in non-test mode due to memory contraints."
#endif  // MOCK_HAL

#include "../io_helpers.h"
#define TEST_PASSED true;

struct TestStatistics {
  uint8_t failed;
  uint8_t passed;
  uint8_t skipped;
};

void combine_statistics(struct TestStatistics *statistics,
                        struct TestStatistics *to_add);

#define SKIP_TEST(test_fn, function_name, test_statistics)       \
  test_statistics.skipped += 1;                                  \
  /* This is the least of the security concerns, so let it be.*/ \
  printf_P(PSTR(function_name));

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       struct TestStatistics *statistics);

#define CHECK_EQ(recieved, expected, fmt)         \
  if ((expected) != (recieved)) {                 \
    dprintf(                                      \
        "\n%s line: %d:\nEquality check failed! " \
        "Expected: " fmt ", Got: " fmt "\n",      \
        __FILE__, __LINE__, expected, recieved);  \
    return false;                                 \
  }

#define CHECK_EQ_MSG(recieved, expected, fmt, msg) \
  if ((expected) == (recieved)) {                  \
    dprintf(                                       \
        "ASSERT_EQUAL ERROR! "                     \
        "Expected: " fmt ", Got: " fmt "\n",       \
        expected, recieved);                       \
    return false;                                  \
  }

#define CHECK_TRUE(recieved)                                               \
  if ((recieved) == false) {                                               \
    dprintf("\n%s line %d:\nCheck failed! Expected 'true' got 'false'!\n", \
            __FILE__, __LINE__);                                           \
    return false;                                                          \
  }
#define CHECK_FALSE(recieved)                                              \
  if ((recieved) == true) {                                                \
    dprintf("\n%s line %d:\nCheck failed! Expected 'false' got 'true'!\n", \
            __FILE__, __LINE__);                                           \
    return false;                                                          \
  }

// Wrapper to put 'function_name' string in program memory.
#define RUN_TEST(fn, test_statistics) \
  run_test_function(fn, PSTR(#fn), test_statistics);

#define dprintf(fmt, ...) debug_printf_flash(PSTR(fmt), ##__VA_ARGS__)
#define dprint_flash_str(str) printf_P(str);

void debug_printf_flash(const char *fmt, ...);
#endif  // defined(TESTS_H_)
