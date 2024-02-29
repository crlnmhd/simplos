#include <string.h>

#include "../hal/log.hpp"
#include "../progmem.hpp"
#include "test.hpp"
#include "test_suite.hpp"

bool test_contains_entry_starting_with_accepts_entry_stored_in_progmem() {
  const ProgmemString full_entry = progmem_string("ABCDEF");
  const ProgmemString progmem_entry = progmem_string("ABC");

  constexpr auto buf_size = 20;
  char buf[buf_size] = {};
  Log log{buf, buf_size};

  CHECK_TRUE(log.add_entry(full_entry));
  CHECK_TRUE(log.contains_entry_starting_with(progmem_entry));

  return TEST_PASSED;
}

bool test_does_not_append_to_log_when_bufferspace_is_insufficiant(void) {
  constexpr uint8_t buf_size = 3;
  char buf[buf_size];
  Log log{buf, buf_size};
  const ProgmemString too_long_entry =
      progmem_string("Hej");  // 4 bytes including null terminator.

  CHECK_FALSE(log.add_entry(too_long_entry));
  return TEST_PASSED;
}

bool test_decreases_remaining_buffer_bytes_by_size_of_mesage_and_terminator(
    void) {
  constexpr uint8_t specified_buffer_size = 10;
  char buf[specified_buffer_size];
  Log log{buf, specified_buffer_size};
  constexpr size_t expected_buffer_size_after_addition = 6;
  const ProgmemString message =
      progmem_string("Hej");  // 4 bytes including null terminator.

  CHECK_TRUE(log.add_entry(message));

  CHECK_EQ(log.available_space_bytes(), expected_buffer_size_after_addition,
           "%zu");

  return TEST_PASSED;
}

bool test_log_contains_entry_finds_entry_in_buffer(void) {
  constexpr uint16_t buffer_size = 10;
  char buffer[buffer_size];
  Log log{buffer, buffer_size};

  const ProgmemString entry = progmem_string("Hejsan");

  CHECK_TRUE(log.add_entry(entry));
  CHECK_TRUE(log.contains_entry(entry));

  return TEST_PASSED;
}

bool test_log_contains_entry_starting_with_returns_true_for_exact_match(void) {
  constexpr uint16_t buffer_size = 10;
  char buffer[buffer_size];
  Log log{buffer, buffer_size};

  const ProgmemString entry = progmem_string("Hejsan");
  CHECK_TRUE(log.add_entry(entry));

  CHECK_TRUE(log.contains_entry_starting_with(entry));

  return TEST_PASSED;
}

bool test_log_contains_entry_starting_with_returns_false_if_the_requested_string_is_empty(
    void) {
  constexpr uint16_t buffer_size = 10;
  char buffer[buffer_size];
  Log log{buffer, buffer_size};

  const ProgmemString entry = progmem_string("Anything");
  const char *empty = "";
  CHECK_TRUE(log.add_entry(entry));

  CHECK_FALSE(log.contains_entry_starting_with(empty));

  return TEST_PASSED;
}

bool test_log_contains_entry_starting_with_returns_true_when_an_entry_starting_with_the_given_string_is_found(
    void) {
  constexpr uint16_t buffer_size = 20;
  char buffer[buffer_size];
  Log log{buffer, buffer_size};

  CHECK_TRUE(log.add_entry(progmem_string("Hello World")));

  CHECK_TRUE(log.contains_entry_starting_with("Hel"));
  CHECK_TRUE(log.contains_entry_starting_with("Hello"));
  CHECK_TRUE(log.contains_entry_starting_with("Hello Wor"));

  return TEST_PASSED;
}

bool test_log_contains_entry_starting_with_returns_false_when_entry_does_not_start_with_given_string(
    void) {
  constexpr uint16_t buffer_size = 20;
  char buffer[buffer_size];
  Log log{buffer, buffer_size};

  const ProgmemString entry1 = progmem_string("ABC123");
  const ProgmemString entry2 = progmem_string("Hello World");
  CHECK_TRUE(log.add_entry(entry1));
  CHECK_TRUE(log.add_entry(entry2));

  CHECK_FALSE(log.contains_entry_starting_with("abc"));
  CHECK_FALSE(log.contains_entry_starting_with("Hej"));
  CHECK_FALSE(log.contains_entry_starting_with("Word"));
  CHECK_FALSE(log.contains_entry_starting_with("ello"));

  return TEST_PASSED;
}

bool test_can_add_multiple_messages(void) {
  constexpr uint16_t specified_buffer_size = 30;
  char buffer[specified_buffer_size];
  Log log{buffer, specified_buffer_size};

  const ProgmemString entry_1 = progmem_string("Hejsan");
  const ProgmemString entry_2 = progmem_string("Svejsan");
  const ProgmemString entry_3 = progmem_string("Voff voff");

  CHECK_TRUE(log.add_entry(entry_1));
  CHECK_TRUE(log.add_entry(entry_2));
  CHECK_TRUE(log.add_entry(entry_3));

  CHECK_TRUE(log.contains_entry(entry_1));
  CHECK_TRUE(log.contains_entry(entry_3));  // try out of order access
  CHECK_TRUE(log.contains_entry(entry_2));

  return TEST_PASSED;
}
bool test_does_not_falsely_find_log_entires_finding_in_log(void) {
  constexpr uint16_t specified_buffer_size = 15;
  char buffer[specified_buffer_size];
  Log log{buffer, specified_buffer_size};

  const ProgmemString real_log_entry = progmem_string("Start");
  const ProgmemString not_in_log = progmem_string("StartingWith");

  CHECK_TRUE(log.add_entry(real_log_entry));
  CHECK_FALSE(log.contains_entry(not_in_log));

  return TEST_PASSED;
}

bool test_available_space_bytes_return_the_size_of_the_buffer_when_it_is_empty(
    void) {
  constexpr uint16_t specified_buffer_size = 15;
  char buffer[specified_buffer_size];
  Log log{buffer, specified_buffer_size};

  CHECK_EQ(log.available_space_bytes(), specified_buffer_size, "%u");

  return TEST_PASSED;
}

bool test_clear_removed_existing_entries(void) {
  constexpr uint16_t specified_buffer_size = 32;
  char buffer[specified_buffer_size];
  Log log{buffer, specified_buffer_size};

  const ProgmemString entry1 = progmem_string("entry 1");
  const ProgmemString entry2 = progmem_string("entry 2");

  log.add_entry(entry1);
  log.add_entry(entry2);

  log.clear();

  CHECK_FALSE(log.contains_entry(entry1));
  CHECK_FALSE(log.contains_entry(entry2));
  CHECK_EQ(log.available_space_bytes(), specified_buffer_size, "%u");

  return TEST_PASSED;
}

TestStatistics unit_test_hal_log(void) {
  TestStatistics test_resutls = {};

  RUN_TEST(test_contains_entry_starting_with_accepts_entry_stored_in_progmem,
           test_resutls);

  RUN_TEST(test_does_not_append_to_log_when_bufferspace_is_insufficiant,
           test_resutls);

  RUN_TEST(
      test_decreases_remaining_buffer_bytes_by_size_of_mesage_and_terminator,
      test_resutls);

  RUN_TEST(test_can_add_multiple_messages, test_resutls);

  RUN_TEST(test_log_contains_entry_finds_entry_in_buffer, test_resutls);

  RUN_TEST(test_log_contains_entry_starting_with_returns_true_for_exact_match,
           test_resutls);

  RUN_TEST(
      test_log_contains_entry_starting_with_returns_false_if_the_requested_string_is_empty,
      test_resutls);

  RUN_TEST(
      test_log_contains_entry_starting_with_returns_true_when_an_entry_starting_with_the_given_string_is_found,
      test_resutls);

  RUN_TEST(
      test_log_contains_entry_starting_with_returns_false_when_entry_does_not_start_with_given_string,
      test_resutls);

  RUN_TEST(test_does_not_falsely_find_log_entires_finding_in_log, test_resutls);

  RUN_TEST(
      test_available_space_bytes_return_the_size_of_the_buffer_when_it_is_empty,
      test_resutls);

  RUN_TEST(test_clear_removed_existing_entries, test_resutls);

  return test_resutls;
}
