#include "log.h"

#ifndef MOCK_HAL
#include "../io_helpers.h"
#endif  // MOCK_HAL

#include <stdint.h>
#include <string.h>

bool add_log_entry(struct Log *log, const char *message) {
  const size_t message_lenght_with_terminator = strlen(message) + 1;
  if (message_lenght_with_terminator > log->num_buffer_bytes_remaining) {
#ifndef MOCK_HAL  // Surpess mesasge in test mode.
    debug_print(
        "Unable to add entry: '%s' to log. Insufficient buffer space.\n",
        message);
#endif  // MOCK_HAL
    return false;
  }
  memcpy(log->end, message, message_lenght_with_terminator);

  log->end += message_lenght_with_terminator;
  log->num_buffer_bytes_remaining -= message_lenght_with_terminator;

  return true;
}

struct Log init_log(char *buffer, const size_t buf_size) {
  // FIXME: Handle insufficient buffer
  struct Log log = {
      .buffer = buffer, .num_buffer_bytes_remaining = buf_size, .end = buffer};
  return log;
}

bool log_contains_entry(struct Log *log, const char *expected_entry) {
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
