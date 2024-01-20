#include "../hal/hal.h"
#include "test.h"
#include "test_suite.h"

#define RUN_TEST_SUITE(entry_fn, name) (run_test_suite(entry_fn, PSTR(name)))

volatile Kernel internal_kernel_location = {0};
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t task_sp = 0;
volatile uint16_t next_task_sp = 0;
volatile uint16_t scheduler_task_sp = 0;

void run_test_suite(struct TestStatistics (*test_entry)(void), PGM_P name) {
  struct TestStatistics test_results = test_entry();
  if (test_results.failed > 0) {
    dprintf("FAILED: %d, ", test_results.failed);
  }
  if (test_results.skipped > 0) {
    dprintf("SKIPPED: %d, ", test_results.skipped);
  }
  dprintf("PASSED: %d\n", test_results.passed);
}
int main(void) {
  uart_init();  // some tests can only be run on an AVR / simulator.
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  RUN_TEST_SUITE(unit_test_hal_log, "Test the testing hal log");

  RUN_TEST_SUITE(unit_test_scheduler, "Scheduler");

  RUN_TEST_SUITE(unit_test_context_switch, "Context switching");

  RUN_TEST_SUITE(unit_test_spawning, "Task spawning");

  dprintf("Test suite completed.\n");
  return 0;
}
