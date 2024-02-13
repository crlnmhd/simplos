#include "../../data_structures/option.hpp"
#include "../test.hpp"
#include "../test_suite.hpp"

bool test_is_valid_returns_true_when_value_is_valid() {
  Option<uint8_t> option_holding_valid_value{5};

  CHECK_TRUE(option_holding_valid_value.is_valid());

  return TEST_PASSED;
}

TestStatistics unit_test_data_structure_optional() {
  TestStatistics results;

  RUN_TEST(test_is_valid_returns_true_when_value_is_valid, results);

  return results;
}
