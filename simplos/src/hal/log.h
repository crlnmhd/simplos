#ifndef LOG_H_
#define LOG_H_
#include <stdbool.h>
#include <stddef.h>

/* Helper functions for the HAL. Can keep and search for entires, matching calls
 * hade though the testing hal.*/
struct Log;

struct Log {
  char *buffer;
  size_t num_buffer_bytes_remaining;
  char *next_available;
};

struct Log init_log(char *buffer, const size_t buf_size);
bool add_log_entry(struct Log *log, const char *entry);
#endif  // LOG_H_
