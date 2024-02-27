#include <stdint.h>

#include "../../data_structures/array.hpp"
#include "../test.hpp"
#include "../test_suite.hpp"

bool test_can_initiate_array_with_default_values() {
  constexpr size_t given_size{5};
  Array<uint8_t, given_size> arr;

  CHECK_TRUE(arr.at(4).is_valid());
  CHECK_EQ(*arr.at(4), 0, "%lu");

  return TEST_PASSED;
}

TestStatistics unit_test_data_structures() {
  TestStatistics results;

  RUN_TEST(test_can_initiate_array_with_default_values, results);

  return results;
}
