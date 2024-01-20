#include <avr/sleep.h>

#include "../hal/hal.h"
#include "../simplos_types.h"
#include "test.h"
#include "test_suite.h"

#define RUN_TEST_SUITE(test_fn, name, statistics) \
  (run_test_suite(test_fn, PSTR(name), statistics))

volatile Kernel internal_kernel_location = {0};
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t task_sp = 0;
volatile uint16_t next_task_sp = 0;
volatile uint16_t prev_task_sp = 0;
volatile uint16_t scheduler_task_sp = 0;

void run_test_suite(struct TestStatistics (*test_fn)(void), PGM_P name,
                    struct TestStatistics *total_results) {
  struct TestStatistics test_suite_results = test_fn();

  if (test_suite_results.failed == 0 && test_suite_results.skipped == 0) {
    debug_printf("PASSED: all.\n");
  } else {
    debug_printf("\n--- %S ---\n", name);
    if (test_suite_results.passed > 0) {
      debug_printf("PASSED: %u.\n", test_suite_results.passed);
    }
    if (test_suite_results.skipped > 0) {
      debug_printf("SKIPPED: %u.\n", test_suite_results.skipped);
    }
    if (test_suite_results.failed > 0) {
      debug_printf("FAILED: %u.\n", test_suite_results.failed);
    }
    debug_printf("\n%S\n\n", PSTR("--- end ---"));  // FIXME???
  }
  combine_statistics(total_results, &test_suite_results);
}

void print_total_test_results(struct TestStatistics *results) {
  if (results->failed == 0) {
    debug_printf("All tests passed\n");
  } else {
    debug_printf("Test failiure(s) encountered. %u test(s) failed\n",
                 results->failed);
  }
  if (results->skipped > 0) {
    debug_printf("Note: %u test(s) were skipped\n", results->skipped);
  }
}

int main(void) {
  uart_init();  // some tests can only be run on an AVR / simulator.
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  struct TestStatistics test_results = {0};

  RUN_TEST_SUITE(unit_test_hal_log, "Test the testing hal log", &test_results);

  RUN_TEST_SUITE(unit_test_scheduler, "Scheduler", &test_results);

  RUN_TEST_SUITE(unit_test_context_switch, "Context switching", &test_results);

  RUN_TEST_SUITE(unit_test_spawning, "Task spawning", &test_results);

  RUN_TEST_SUITE(unit_test_memory, "Memory", &test_results);

  debug_printf("Test suite completed.\n", &test_results);
  print_total_test_results(&test_results);

  // sleeping with interupts disabled triggers simavr to exit.
  asm volatile("cli" ::: "memory");
  sleep_cpu();
}
