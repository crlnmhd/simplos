#ifndef ARRAY_HPP_
#define ARRAY_HPP_

#include <stddef.h>

#include "../build_config.hpp"
#include "option.hpp"

/* NOTE:
 * Child classes must *not* attempt to call delete() though a pointer to this
 * base class as is common in systems with dynamic memory allocation. That would
 * invoke UB.
 *
 * The protected non-virtual destructor avoids the undesierable default
 * behaviour of virtual destructors (i.e. attempting to call delete).
 *
 * */
template <typename T>
class Indexable {
 public:
  Option<T> at(const size_t index) const {
    return this->bounds_checked_access(index);
  };

 protected:
  virtual Option<T> bounds_checked_access(const size_t index) const = 0;
  ~Indexable() = default;
};

// TODO: compile time bound checking when possible. The size is known at compile
// time, so many of the checks are unnessesary.
GCC_IGNORE_BEGIN("-Wnon-virtual-dtor")
CLANG_IGNORE_BEGIN("-Wnon-virtual-dtor")
template <typename T, size_t Size>
class Array : public Indexable<T> {
 public:
  Array() : data(){};

  ~Array() = default;

 private:
  Option<T> bounds_checked_access(const size_t index) const override {
    if (index < Size) {
      return Option<T>(data[index]);
    } else {
      return Option<T>{};
    }
  }

  T data[Size];
};
GCC_IGNORE_END()
CLANG_IGNORE_END()

#endif  // ARRAY_HPP_
