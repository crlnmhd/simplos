#include "test.h"

void run_test_function(bool (*fn_ptr)(void), PGM_P function_name,
                       struct TestStatistics *statistics) {
  if (fn_ptr()) {
    statistics->passed = (uint8_t)(statistics->passed + 1U);
  } else {
    debug_printf("%S", function_name);
    debug_printf(" --- FAILED!!\n");
    statistics->failed = (uint8_t)(statistics->failed + 1U);
  }
}

void combine_statistics(struct TestStatistics *statistics,
                        struct TestStatistics *to_add) {
  statistics->passed = (uint8_t)(statistics->passed + to_add->passed);
  statistics->failed = (uint8_t)(statistics->failed + to_add->failed);
  statistics->skipped = (uint8_t)(statistics->skipped + to_add->skipped);
}

void debug_printf_flash(const char *fmt, ...) {
  uint8_t sreg_at_entry = SREG;
  va_list args;
  va_start(args, fmt);
  vfprintf_P(stdout, fmt, args);
  va_end(args);
  SREG = sreg_at_entry;
}
