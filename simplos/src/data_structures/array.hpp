#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <stddef.h>

#include "option.hpp"

template <typename T, size_t Size>
class Array {
 public:
  Array() : data(){};

  Option<T> at(const size_t index) const {
    if (index < Size) {
      return Option(data[index]);
    } else {
      return Option<T>{};
    }
  }

 private:
  T data[Size];
};

#endif  // ARRAY_HPP_
