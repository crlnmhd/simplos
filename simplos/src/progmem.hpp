#ifndef PROGMEM_HPP_
#define PROGMEM_HPP_

#include <avr/pgmspace.h>

struct ProgmemStringWrapper {
  const char *progmem_string;
};

struct ProgmemString {
  ProgmemString() : progmem_str(nullptr) {}
  explicit ProgmemString(const ProgmemStringWrapper &wrapper)
      : progmem_str(wrapper.progmem_string) {}
  explicit ProgmemString(const ProgmemString &other)
      : progmem_str(other.progmem_str) {}
  ProgmemString(ProgmemString &&other) : progmem_str(other.progmem_str) {}
  ProgmemString &operator=(const ProgmemString &other) {
    if (this != &other) {
      this->progmem_str = other.progmem_str;
    }
    return *this;
  }

  const char *progmem_str PROGMEM;
};

#define progmem_string(str_literal) \
  ProgmemString { static_cast<const ProgmemStringWrapper>(PSTR((str_literal))) }

#endif  // PROGMEM_HPP_
