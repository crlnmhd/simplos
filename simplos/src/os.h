#if !defined(OS_H_)
#define OS_H_

#if defined(__clang__)
// clang-format off
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif

#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>

typedef uint16_t pid_t;

// clang-format on

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

#define print(fmt, ...)                    \
  do {                                     \
    uint8_t internal_kernel_sreg__ = SREG; \
    cli();                                 \
    printf_P(PSTR(fmt), ##__VA_ARGS__);    \
    SREG = internal_kernel_sreg__;         \
  } while (0)

#define HALT_EXEC()  \
  print("Halting!"); \
  cli();             \
  for (;;)           \
    ;

#endif  // OS_H_
