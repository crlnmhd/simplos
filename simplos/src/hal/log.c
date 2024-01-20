#include "log.h"

#include <string.h>

#include "../tests/test.h"

bool add_log_entry(struct Log *log, const char *message) {
  const size_t message_lenght_with_terminator = strlen(message) + 1;
  if (message_lenght_with_terminator > log->num_buffer_bytes_remaining) {
    // Surpess mesasge in tes mode.
#ifndef MOCK_HAL
    dprintf("Unable to add entry: '%s' to log. Insufficient buffer space.\n",
            message);
#endif  // MOCK_HAL
    return false;
  }
  return true;
}

struct Log init_log(char *buffer, const size_t buf_size) {
  struct Log log = {.buffer = buffer,
                    .num_buffer_bytes_remaining = buf_size,
                    .next_available = buffer};
  return log;
}
