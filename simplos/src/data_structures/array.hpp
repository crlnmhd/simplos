#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <stddef.h>

template <typename T, size_t S>
class Array {
 public:
  T data[S];
};

#endif  // ARRAY_HPP_
