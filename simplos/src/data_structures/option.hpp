#ifndef OPTION_HPP_
#define OPTION_HPP_

#include "../hal/hal.hpp"

template <typename T>
class Option {
 public:
  explicit Option(T valid_value)
      : status(Status::VALID), maybe_value(valid_value) {}
  Option() : status(Status::INVALID), maybe_value() {}

  bool is_valid() const { return status == Status::VALID; }
  T operator*() const {
    if (!is_valid()) {
      FATAL_ERROR("Attempted access of option in invalid state");
    }
    return maybe_value;
  }

 private:
  enum class Status {
    VALID,
    INVALID,
  };

  Status status;
  const T maybe_value;
};

#endif  // OPTION_HPP_
