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
        next_entry_index(0),
        num_buffer_entries_remaining(buf_size),
        max_entries(buf_size) {
    // FIXME: templetaize size
  }

  bool add_entry(const ProgmemString &entry);
  bool contains_entry(const ProgmemString &expected_entry) const;
  bool contains_entry_starting_with(const ProgmemString &expected_entry) const;
  void clear();
  size_t num_unused_entries() const;

 private:
  bool add_entry(const char *message);
  bool contains_entry(const char *expected_entry) const;
  bool contains_entry_starting_with(const char *expected_entry) const;
  char *buffer;
  size_t next_entry_index;
  size_t num_buffer_entries_remaining;
  size_t max_entries;
};

#endif  // LOG_H_
