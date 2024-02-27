#include "log.hpp"

#include <stdint.h>
#include <string.h>

#include "../io_helpers.hpp"

bool Log::add_entry(const char *message) {
  const size_t message_lenght_with_terminator = strlen(message) + 1;
  if (message_lenght_with_terminator > this->num_buffer_bytes_remaining) {
#ifndef MOCK_HAL  // Surpess mesasge in test mode.
    WARNING("Unable to add entry: '%s' to log. Insufficient buffer space.\n",
            message);
#endif  // MOCK_HAL
    return false;
  }
  memcpy(this->end, message, message_lenght_with_terminator);

  this->end += message_lenght_with_terminator;
  this->num_buffer_bytes_remaining -= message_lenght_with_terminator;

  return true;
}

bool Log::contains_entry(const char *expected_entry) const {
  const auto expected_entry_length = strlen(expected_entry);

  char *next_entry = this->buffer;
  uint16_t next_entry_length = strlen(next_entry);
  while (next_entry + next_entry_length <= this->end) {
    if (expected_entry_length == next_entry_length) {
      const int comparison_found_and_expected =
          strcmp(next_entry, expected_entry);
      if (comparison_found_and_expected == 0) {
        return true;
      }
    }
    next_entry += next_entry_length + 1;
    next_entry_length = strlen(next_entry);
  }
  return false;
}

bool Log::contains_entry_starting_with(const char *expected_entry) const {
  const auto expected_entry_length = strlen(expected_entry);
  /* Special case, don't match for empty string. Otherwise everything would be a
   * match.*/
  if (expected_entry_length == 0) {
    return false;
  }

  char *next_entry = this->buffer;
  uint16_t next_entry_length = strlen(next_entry);
  while (next_entry + next_entry_length <= this->end) {
    const int comparison_found_and_expected =
        strncmp(next_entry, expected_entry, expected_entry_length);

    if (comparison_found_and_expected == 0) {
      return true;
    } else {
      next_entry += next_entry_length + 1;
      next_entry_length = strlen(next_entry);
    }
  }
  return false;
}

size_t Log::available_space_bytes() const {
  return this->num_buffer_bytes_remaining;
}

void Log::clear() {
  this->end = this->buffer;
  this->num_buffer_bytes_remaining = this->total_buffer_bytes;
}
