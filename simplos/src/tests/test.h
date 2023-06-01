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

void run_tests(void);

/*
 * Tests that run prior to initilizing the scheduler.
 * Intened to test key functionality needed by the setup process.
 * */
void pre_scheduler_self_checks(void);

#endif  // defined(TESTS_H_)
