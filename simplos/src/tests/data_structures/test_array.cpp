#include <stdint.h>

#include "../../data_structures/array.hpp"
#include "../test.hpp"
#include "../test_suite.hpp"

bool test_can_initiate_array_with_default_values() {
  constexpr size_t given_size{3};
  Array<uint8_t, given_size> arr;

  CHECK_TRUE(arr.at(0).is_valid());
  CHECK_EQ(*arr.at(0), 0, "%lu");

  CHECK_TRUE(arr.at(1).is_valid());
  CHECK_EQ(*arr.at(1), 0, "%lu");

  CHECK_TRUE(arr.at(2).is_valid());
  CHECK_EQ(*arr.at(2), 0, "%lu");

  return TEST_PASSED;
}

bool test_returns_optional_holding_invalid_when_index_is_out_of_bounds() {
  constexpr size_t given_size{5};
  Array<uint8_t, given_size> arr;

  CHECK_FALSE(arr.at(given_size).is_valid());

  return TEST_PASSED;
}

TestStatistics unit_test_data_structures() {
  TestStatistics results;

  RUN_TEST(test_can_initiate_array_with_default_values, results);

  RUN_TEST(test_returns_optional_holding_invalid_when_index_is_out_of_bounds,
           results);

  return results;
}
