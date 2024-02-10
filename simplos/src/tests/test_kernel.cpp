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
  constexpr char expected_task_name[] = "Task 1";

  kernel.set_task_name_P(task_index, progmem_string("Task 1"));
  Simplos_Task *task = &kernel.schedule.queue.tasks[task_index];

  const bool task_name_is_equal_to_expected =
      !strcmp_P(expected_task_name,
                task->name.progmem_str);  // bounded strncmp  not available

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
