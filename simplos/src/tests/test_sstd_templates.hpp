#ifndef TEST_SSTD_TEMPLATESS_HPP_
#define TEST_SSTD_TEMPLATESS_HPP_

#include "../sstd.hpp"
#include "stdint.h"

template <typename U8,
          sstd::enable_if_t<sstd::is_same<U8, uint8_t>::value, bool> = true>
int return_1_for_uint8_t_and_2_for_uint16(U8) {
  return 1;
}

template <typename U16,
          sstd::enable_if_t<sstd::is_same<U16, uint16_t>::value, bool> = true>
int return_1_for_uint8_t_and_2_for_uint16(U16) {
  return 2;
}

#endif  // TEST_SSTD_TEMPLATESS_HPP_
