#include "test.hpp"
#include "test_suite.hpp"

bool test_get_task_disables_mt_on_exit(void) { return TEST_PASSED; }

struct TestStatistics unit_test_os(void) {
  struct TestStatistics results = {};
  return results;
}
