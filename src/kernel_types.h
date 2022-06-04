#ifndef KERNEL_TYPLES_H_
#define KERNEL_TYPLES_H_

#include <stdint.h>

// TODO use this for all global variables.
typedef struct Kernel_t {
  uint8_t heap_mapping[HEAP_CHUNKS];
} Kernel_t;

typedef Kernel_t volatile *volatile Kernel;

#endif  // KERNEL_TYPLES_H_
