#include "test.hpp"
#include "test_suite.hpp"

bool test_task_is_initilized_with_status_empty() {
  Simplos_Task task{};

  CHECK_EQ(task.status, Task_Status::EMPTY, "%u");

  return TEST_PASSED;
}

bool test_set_name_sets_name_of_task() {
  Kernel kernel{};
  constexpr char expected_task_name[] = "Task 1";
  Simplos_Task task{};
  task.set_name(progmem_string("Task 1"));

  const bool task_name_is_equal_to_expected =
      !strcmp_P(expected_task_name,
                task.name.progmem_str);  // bounded strncmp  not available

  CHECK_TRUE(task_name_is_equal_to_expected);
  return TEST_PASSED;
}

TestStatistics unit_test_tasks() {
  TestStatistics results{};

  RUN_TEST(test_task_is_initilized_with_status_empty, results)

  RUN_TEST(test_set_name_sets_name_of_task, results);

  return results;
}
