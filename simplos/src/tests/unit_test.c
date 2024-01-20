#include "../hal/hal.h"
#include "../simplos_types.h"
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
  if (test_results.failed == 0 && test_results.skipped == 0) {
    debug_printf("PASSED: all.\n");
  } else {
    debug_printf("\n--- %S ---\n", name);
    if (test_results.passed > 0) {
      debug_printf("PASSED: %u.\n", test_results.passed);
    }
    if (test_results.skipped > 0) {
      debug_printf("SKIPPED: %u.\n", test_results.skipped);
    }
    if (test_results.failed > 0) {
      debug_printf("FAILED: %u.\n", test_results.failed);
    }
    debug_printf("\n%S\n\n", PSTR("--- end ---"));  // FIXME???
  }
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

  RUN_TEST_SUITE(unit_test_memory, "Memory");

  debug_printf("Test suite completed.\n");
  return 0;
}
