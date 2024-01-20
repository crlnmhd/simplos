#include "../hal/log.h"
#include "test.h"
#include "test_suite.h"

bool test_does_not_append_to_log_when_bufferspace_is_insufficiant(void) {
  const uint8_t buf_size = 3;
  char buf[buf_size];
  struct Log log = init_log(buf, buf_size);
  const char *too_long_message = "Hej";  // 4 bytes including null terminator.

  const bool successfully_added = add_log_entry(&log, too_long_message);

  CHECK_EQ(successfully_added, false, "%d");
  return true;
}

struct TestStatistics unit_test_hal_log(void) {
  struct TestStatistics test_resutls = {0};

  RUN_TEST(test_does_not_append_to_log_when_bufferspace_is_insufficiant,
           &test_resutls);

  return test_resutls;
}
