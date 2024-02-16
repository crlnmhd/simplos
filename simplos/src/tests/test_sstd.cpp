
#include "../sstd.hpp"
#include "test.hpp"
#include "test_sstd_templates.hpp"
#include "test_suite.hpp"

bool test_tow_uint8_ts_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<uint8_t, uint8_t>();

  CHECK_TRUE(same_type);

  return TEST_PASSED;
}
bool test_tow_uint16_ts_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<uint16_t, uint16_t>();

  CHECK_TRUE(same_type);

  return TEST_PASSED;
}
bool test_tow_uint32_ts_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<uint32_t, uint32_t>();

  CHECK_TRUE(same_type);
  return TEST_PASSED;
}
bool test_tow_cstirngs_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<const char *, const char *>();

  CHECK_TRUE(same_type);

  return TEST_PASSED;
}

bool test_classes_of_the_same_type_have_the_same_type() {
  class A {};
  constexpr auto same_type = sstd::is_same<A, A>();

  CHECK_TRUE(same_type);

  return TEST_PASSED;
}

bool test_different_classes_do_not_have_the_same_type() {
  class A {};
  class B {};
  constexpr auto same_type = sstd::is_same<A, B>();

  CHECK_FALSE(same_type);

  return TEST_PASSED;
}

bool test_child_class_does_not_have_the_same_type_as_base() {
  class Base {};
  class Child : Base {};
  constexpr auto same_type = sstd::is_same<Child, Base>();

  CHECK_FALSE(same_type);

  return TEST_PASSED;
}

bool test_uint8_and_int8_do_not_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<uint8_t, int8_t>();

  CHECK_FALSE(same_type);

  return TEST_PASSED;
}

bool test_uint8_and_int16_do_not_have_the_same_type() {
  constexpr auto same_type = sstd::is_same<uint8_t, int16_t>();

  CHECK_FALSE(same_type);

  return TEST_PASSED;
}

/*
 * Test that simple useage of sstd::enable_if (used in test_sstd_templates.hpp)
 * works as expected.
 * */
bool test_template_enable_if_can_distinguish_between_passed_types_when_value_is_the_same() {
  const uint8_t u8{42};
  const uint16_t u16{42};

  CHECK_EQ(return_1_for_uint8_t_and_2_for_uint16(u8), 1, "%u");
  CHECK_EQ(return_1_for_uint8_t_and_2_for_uint16(u16), 2, "%u");

  return TEST_PASSED;
}

TestStatistics unit_test_sstd() {
  TestStatistics results;

  RUN_TEST(test_tow_uint8_ts_have_the_same_type, results);

  RUN_TEST(test_tow_uint16_ts_have_the_same_type, results);

  RUN_TEST(test_tow_uint32_ts_have_the_same_type, results);

  RUN_TEST(test_tow_cstirngs_have_the_same_type, results);

  RUN_TEST(test_uint8_and_int8_do_not_have_the_same_type, results);

  RUN_TEST(test_uint8_and_int16_do_not_have_the_same_type, results);

  RUN_TEST(test_classes_of_the_same_type_have_the_same_type, results);

  RUN_TEST(test_different_classes_do_not_have_the_same_type, results);

  RUN_TEST(
      test_template_enable_if_can_distinguish_between_passed_types_when_value_is_the_same,
      results);

  return results;
}
