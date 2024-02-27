#include "../../data_structures/option.hpp"
#include "../test.hpp"
#include "../test_suite.hpp"

bool test_is_valid_returns_true_when_value_is_valid() {
  Option<uint8_t> option_holding_valid_value{5};

  CHECK_TRUE(option_holding_valid_value.is_valid());

  return TEST_PASSED;
}

bool test_is_valid_returns_false_when_optional_is_invalid() {
  Option<uint8_t> option_in_invalid_state{};

  CHECK_FALSE(option_in_invalid_state.is_valid());

  return TEST_PASSED;
}

bool test_operator_star_returns_the_value_of_a_valid_option() {
  constexpr uint8_t expected_value{42};
  Option<uint8_t> valid{expected_value};

  CHECK_EQ(*valid, expected_value, "u");

  return TEST_PASSED;
}

bool test_operator_star_calls_FATAL_ERROR_if_operator_star_is_used_on_invalid_struct() {
  Option<uint8_t> invalid{};

  hal_log.clear();
  __attribute__((unused)) auto something_invalid = *invalid;

  CHECK_TRUE(hal_log.contains_entry(
      "FATAL ERROR\nAttempted access of option in invalid state"));
  hal_log.clear();

  return TEST_PASSED;
}

TestStatistics unit_test_data_structure_optional() {
  TestStatistics results;
  RUN_TEST(test_is_valid_returns_true_when_value_is_valid, results);

  RUN_TEST(test_is_valid_returns_false_when_optional_is_invalid, results);

  RUN_TEST(test_operator_star_returns_the_value_of_a_valid_option, results);

  RUN_TEST(
      test_operator_star_calls_FATAL_ERROR_if_operator_star_is_used_on_invalid_struct,
      results);

  return results;
}
