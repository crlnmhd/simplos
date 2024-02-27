#ifndef PROGMEM_HPP_
#define PROGMEM_HPP_

#include <avr/pgmspace.h>

struct ProgmemStringWrapper {
  const char *progmem_string;
};

class ProgmemString {
 public:
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

#define print_from_prg_mem(fmt, ...) printf_flash(PSTR(fmt), ##__VA_ARGS__)

#endif  // PROGMEM_HPP_
