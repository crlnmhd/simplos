#include "test.hpp"
#include "test_suite.hpp"

bool test_task_is_initilized_with_status_empty() {
  Simplos_Task task{};

  CHECK_EQ(task.status, Task_Status::EMPTY, "%u");

  return TEST_PASSED;
}

struct TestStatistics unit_test_tasks() {
  struct TestStatistics results {};

  RUN_TEST(test_task_is_initilized_with_status_empty, &results)

  return results;
}
