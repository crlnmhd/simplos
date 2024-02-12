#include <string.h>

#include "test.hpp"
#include "test_suite.hpp"

bool test_kernel_data_is_initilized_with_pid_counter_set_to_zero() {
  Kernel kernel{};

  CHECK_EQ(kernel.pid_cnt, 0, "%lu");

  return TEST_PASSED;
}

struct TestStatistics unit_test_kernel_status() {
  struct TestStatistics results {};

  RUN_TEST(test_kernel_data_is_initilized_with_pid_counter_set_to_zero,
           &results);

  return results;
}
