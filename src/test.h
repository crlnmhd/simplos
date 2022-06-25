#include <math.h>
#include <stdint.h>
#if !defined(TESTS_H_)
#define TESTS_H_
#include <stdbool.h>

struct TestStatistics {
  uint8_t failed;
  uint8_t passed;
  uint8_t skipped;
};

struct TestStatistics run_all_tests(void);
#endif  // defined(TESTS_H_)
