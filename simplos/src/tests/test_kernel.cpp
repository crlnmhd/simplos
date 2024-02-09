#include <string.h>

#include "test.hpp"
#include "test_suite.hpp"

bool test_kernel_data_is_initilized_with_pid_counter_set_to_zero() {
  Kernel kernel{};

  CHECK_EQ(kernel.pid_cnt, 0, "%lu");

  return TEST_PASSED;
}

bool test_set_name_sets_name_of_task() {
  Kernel kernel{};
  constexpr uint8_t task_index{3};
  const char *given_task_name = "Task 1";

  kernel.set_task_name(task_index, given_task_name);

  const bool task_name_is_equal_to_expected =
      !strcmp(kernel.task_names[task_index],
              given_task_name);  // bounded strncmp  not available

  CHECK_TRUE(task_name_is_equal_to_expected);
  return TEST_PASSED;
}

struct TestStatistics unit_test_kernel_status() {
  struct TestStatistics results {};

  RUN_TEST(test_kernel_data_is_initilized_with_pid_counter_set_to_zero,
           &results);

  RUN_TEST(test_set_name_sets_name_of_task, &results);
  return results;
}
