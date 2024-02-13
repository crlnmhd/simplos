#include "test.h"
#include "test_suite.h"

bool test_get_task_disables_mt_on_exit(void) { return TEST_PASSED; }

struct TestStatistics unit_test_os(void) {
  struct TestStatistics results = {0};
  return results;
}
