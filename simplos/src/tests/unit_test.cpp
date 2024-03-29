#include "../hal/hal.hpp"
#include "../simplos_types.hpp"
#include "test.hpp"
#include "test_suite.hpp"

// Workaround due to avr-libc issue #898
// https://github.com/avrdudes/avr-libc/issues/898
#define RUN_TEST_SUITE(test_fn, statistics) \
  (run_test_suite(test_fn, PSTR(#test_fn), statistics))

// sleeping with interupts disabled triggers simavr to exit.
#define EXIT_SIMULATOR()          \
  asm volatile(                   \
      "cli                  \n\t" \
      "sleep                \n\t" \
      : /* No outputs*/           \
      : /* No inputs */           \
      : "memory");

volatile Kernel internal_kernel_location = {};
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t task_sp = 0;
volatile uint16_t next_task_sp = 0;
volatile uint16_t prev_task_sp = 0;
volatile uint16_t scheduler_task_sp = 0;

void run_test_suite(TestStatistics (*test_fn)(void), PGM_P name,
                    TestStatistics &total_results) {
  TestStatistics test_suite_results = test_fn();

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
  combine_statistics(total_results, test_suite_results);
}

void print_total_test_results(TestStatistics &results) {
  if (results.failed == 0) {
    debug_printf("All tests passed\n");
  } else {
    debug_printf("Test failiure(s) encountered. %u test(s) failed\n",
                 results.failed);
  }
  if (results.skipped > 0) {
    debug_printf("Note: %u test(s) were skipped\n", results.skipped);
  }
}

int main(void) {
  uart_init();  // some tests can only be run on an AVR / simulator.
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  TestStatistics test_results = {};

  RUN_TEST_SUITE(unit_test_hal_log, test_results);

  RUN_TEST_SUITE(unit_test_tasks, test_results);

  RUN_TEST_SUITE(unit_test_scheduler, test_results);

  RUN_TEST_SUITE(unit_test_kernel_status, test_results);

  RUN_TEST_SUITE(unit_test_context_switch, test_results);

  RUN_TEST_SUITE(unit_test_spawning, test_results);

  RUN_TEST_SUITE(unit_test_memory, test_results);

  RUN_TEST_SUITE(unit_test_os, test_results);

  RUN_TEST_SUITE(unit_test_data_structures, test_results);

  RUN_TEST_SUITE(unit_test_data_structure_optional, test_results);

  RUN_TEST_SUITE(unit_test_sstd, test_results);

  debug_printf("Test suite completed.\n");
  print_total_test_results(test_results);

  EXIT_SIMULATOR();
}
