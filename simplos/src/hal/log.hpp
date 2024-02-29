#ifndef LOG_H_
#define LOG_H_
#include <stdbool.h>
#include <stddef.h>

#include "../progmem.hpp"

/* Helper functions for the testing HAL. Can keep and search for entires,
 * matching calls made though the testing hal withouth causing side effects.
 * */

class Log {
 public:
  Log(char *buf, const size_t buf_size)
      : buffer(buf),
        end(buffer),
        num_buffer_bytes_remaining(buf_size),
        total_buffer_bytes(buf_size) {
    // FIXME: templetaize size
  }

  bool add_entry(const ProgmemString &entry);
  bool add_entry(const char *message);
  bool contains_entry(const ProgmemString &expected_entry) const;
  bool contains_entry(const char *expected_entry) const;
  bool contains_entry_starting_with(const ProgmemString &expected_entry) const;
  bool contains_entry_starting_with(const char *expected_entry) const;
  void clear();
  size_t available_space_bytes() const;

 private:
  char *buffer;
  char *end;
  size_t num_buffer_bytes_remaining;
  size_t total_buffer_bytes;
};

#endif  // LOG_H_
