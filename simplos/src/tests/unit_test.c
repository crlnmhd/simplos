#include "test.h"
#include "test_suite.h"

#define RUN_TEST_SUITE(entry_fn, name) (run_test_suite(entry_fn, PSTR(name)))

volatile Kernel internal_kernel_location = {0};
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t task_sp = 0;
volatile uint16_t next_task_sp = 0;
volatile uint16_t scheduler_task_sp = 0;

void run_test_suite(struct TestStatistics (*test_entry)(void),
                    const char *name) {
  print("Testing: %s ... ", name);
  struct TestStatistics test_results = test_entry();
  if (test_results.failed > 0) {
    print("FAILED: %d ", test_results.failed);
  }
  if (test_results.failed > 0) {
    print("SKIPPED: %d ", test_results.skipped);
  }
  print("PASSED: %d ", test_results.passed);
}
int main(void) {}
