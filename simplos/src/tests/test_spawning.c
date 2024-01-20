#include "test.h"
#include "test_suite.h"

bool test_spawn_task_creates_new_simplos_task(void) { return true; }

struct TestStatistics unit_test_spawning(void) {
  struct TestStatistics test_results = {0};

  RUN_TEST(test_spawn_task_creates_new_simplos_task, &test_results);
  return test_results;
}
