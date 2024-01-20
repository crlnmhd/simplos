#ifndef LOG_H_
#define LOG_H_
#include <stdbool.h>
#include <stddef.h>

/* Helper functions for the testing HAL. Can keep and search for entires,
 * matching calls made though the testing hal withouth causing side effects.
 * */

struct Log {
  char *buffer;
  size_t num_buffer_bytes_remaining;
  char *end;
};

struct Log init_log(char *buffer, const size_t buf_size);
bool add_log_entry(struct Log *log, const char *entry);
bool log_contains_entry(struct Log *log, const char *expected_entry);
#endif  // LOG_H_
