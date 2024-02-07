#ifndef LOG_H_
#define LOG_H_
#include <stdbool.h>
#include <stddef.h>

/* Helper functions for the testing HAL. Can keep and search for entires,
 * matching calls made though the testing hal withouth causing side effects.
 * */

class Log {
 public:
  Log(char *buf, const size_t buf_size)
      : buffer(buf), end(buffer), num_buffer_bytes_remaining(buf_size) {
    // FIXME: Handle insufficient buffer
  }

  char *buffer;
  char *end;
  size_t num_buffer_bytes_remaining;
};

bool add_log_entry(Log *log, const char *entry);
bool log_contains_entry(Log *log, const char *expected_entry);
#endif  // LOG_H_
