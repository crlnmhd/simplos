#include <stdint.h>

#include "../../data_structures/array.hpp"
#include "../test.hpp"
#include "../test_suite.hpp"

bool test_can_initiate_array_with_specified_size() {
  constexpr size_t given_size{5};
  Array<uint8_t, given_size> arr;

  CHECK_EQ(sizeof(arr.data) / sizeof(arr.data[0]), given_size, "%lu");

  return TEST_PASSED;
}

TestStatistics unit_test_data_structures() {
  TestStatistics results;

  RUN_TEST(test_can_initiate_array_with_specified_size, results);

  return results;
}
