#if !defined(OS_H_)
#define OS_H_

#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
    // #include "simplos.h"

    // #define yield() k_yield();
    typedef uint16_t pid_t;

// typedef struct mutex {
//   volatile
// } mutex;

void yield(void);
// uint8_t fork(void);
uint8_t rank(void);
pid_t pid(void);
pid_t spawn(void (*fn)(void), uint8_t const priority, char const *name);
void set_priority(uint8_t const priority);
void kill_curr_task(void);

void wait_for_task_finnish(pid_t);

void terminate(void);
/*
 * Attempts to allocate 'bytes' of continuous memory. Returns NULL on failure.
 * */
void *malloc(size_t bytes);

// mutex* mutex_create(void);
// void mutex_lock(mutex* mtx);
// void mutex_unlock(mutex* mtx);

#define print(fmt, ...)               \
  asm volatile("cli");                \
  printf_P(PSTR(fmt), ##__VA_ARGS__); \
  asm volatile("sei");

#define HALT_EXEC()    \
  print("Halting!");   \
  asm volatile("cli"); \
  for (;;)             \
    ;

#endif  // OS_H_
