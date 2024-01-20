#include "../hal/log.h"
#include "string.h"
#include "test.h"
#include "test_suite.h"

bool test_does_not_append_to_log_when_bufferspace_is_insufficiant(void) {
  const uint8_t buf_size = 3;
  char buf[buf_size];
  struct Log log = init_log(buf, buf_size);
  const char *too_long_message = "Hej";  // 4 bytes including null terminator.

  CHECK_FALSE(add_log_entry(&log, too_long_message));
  return true;
}

bool test_decreases_remaining_buffer_bytes_by_size_of_mesage_and_terminator(
    void) {
  const uint8_t sufficient_buffer_size = 10;
  char buf[sufficient_buffer_size];
  struct Log log = init_log(buf, sufficient_buffer_size);
  const size_t expected_buffer_size_after_addition = 6;
  const char *message = "Hej";  // 4 bytes including null terminator.

  CHECK_TRUE(add_log_entry(&log, message));
  CHECK_EQ(log.num_buffer_bytes_remaining, expected_buffer_size_after_addition,
           "%d");

  return true;
}

bool test_adds_message_to_the_end_of_the_given_buffer(void) {
  const uint8_t sufficient_buffer_size = 10;
  char buf[sufficient_buffer_size];
  struct Log log = init_log(buf, sufficient_buffer_size);
  const size_t expected_message_lenght = 4;
  const char *message = "Hej";  // 4 bytes including null terminator.

  CHECK_TRUE(add_log_entry(&log, message));

  const int16_t memcmp_buffer_and_expected_string =
      memcmp(log.buffer, message, expected_message_lenght);
  const bool buffer_starts_with_expected_message =
      (memcmp_buffer_and_expected_string == 0);

  return buffer_starts_with_expected_message;
}

bool test_log_contains_entry_finds_entry_in_buffer(void) {
  const uint16_t buffer_size = 10;
  char buffer[buffer_size];
  struct Log log = init_log(buffer, buffer_size);

  const char *entry = "Hejsan";

  CHECK_TRUE(add_log_entry(&log, entry));
  const bool b = log_contains_entry(&log, entry);
  CHECK_EQ(b, true, "%d");

  return true;
}

bool test_can_add_multiple_messages(void) {
  const uint16_t sufficient_buffer_size = 30;
  char buffer[sufficient_buffer_size];
  struct Log log = init_log(buffer, sufficient_buffer_size);

  const char *entry_1 = "Hejsan";
  const char *entry_2 = "Svejsan";
  const char *entry_3 = "Voff voff";

  CHECK_TRUE(add_log_entry(&log, entry_1));
  CHECK_TRUE(add_log_entry(&log, entry_2));
  CHECK_TRUE(add_log_entry(&log, entry_3));

  CHECK_TRUE(log_contains_entry(&log, entry_1));
  CHECK_TRUE(log_contains_entry(&log, entry_3));  // try out of order access
  CHECK_TRUE(log_contains_entry(&log, entry_2));

  return true;
}
bool test_does_not_falsely_find_log_entires_finding_in_log(void) {
  const uint16_t sufficient_buffer_size = 15;
  char buffer[sufficient_buffer_size];
  struct Log log = init_log(buffer, sufficient_buffer_size);

  const char *real_log_entry = "Start";
  const char *not_in_log = "StartingWith";

  CHECK_TRUE(add_log_entry(&log, real_log_entry));
  CHECK_FALSE(log_contains_entry(&log, not_in_log));

  return true;
}

struct TestStatistics unit_test_hal_log(void) {
  struct TestStatistics test_resutls = {0};

  RUN_TEST(test_does_not_append_to_log_when_bufferspace_is_insufficiant,
           &test_resutls);

  RUN_TEST(
      test_decreases_remaining_buffer_bytes_by_size_of_mesage_and_terminator,
      &test_resutls);

  RUN_TEST(test_adds_message_to_the_end_of_the_given_buffer, &test_resutls);

  RUN_TEST(test_can_add_multiple_messages, &test_resutls);

  RUN_TEST(test_log_contains_entry_finds_entry_in_buffer, &test_resutls);

  RUN_TEST(test_does_not_falsely_find_log_entires_finding_in_log,
           &test_resutls);

  return test_resutls;
}
