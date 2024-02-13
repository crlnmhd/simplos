#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <stddef.h>

template <typename T, size_t Size>
class Array {
 public:
  Array() : data(){};

  T at(const size_t index) {
    if (index < Size) {
      return data[index];
    } else {
      return T{};  // FIXME
    }
  }

 private:
  T data[Size];
};

#endif  // ARRAY_HPP_
