#include "log.h"

#ifndef MOCK_HAL
#include "../io_helpers.h"
#endif  // MOCK_HAL

#include <stdint.h>
#include <string.h>

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

bool add_log_entry(Log *log, const char *message) {
  return log->add_entry(message);
}

bool log_contains_entry(Log *log, const char *expected_entry) {
  const uint16_t expected_entry_length = strlen(expected_entry);

  char *next_entry = log->buffer;
  uint16_t next_entry_length = strlen(next_entry);
  while (next_entry + next_entry_length <= log->end) {
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
