#include "test.h"

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       struct TestStatistics statistics) {
  print_from_prg_mem("Testing: ");
  printf_P(function_name);
  print_from_prg_mem("\n");
  if (!fn_ptr()) {
    statistics.failed = (uint8_t)(statistics.failed + 1U);
    printf_P(function_name);  // In an 'OS' with so many security concerns, this
                              // is the least of our problems.
    print_from_prg_mem(" failed");
  } else {
    statistics.passed = (uint8_t)(statistics.passed + 1U);
  }
}
