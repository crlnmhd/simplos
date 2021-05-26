#ifndef OS_H_
#define OS_H_

#include <avr/pgmspace.h>
#include <stdint.h>

// #include "simplos.h"

// #define yield() k_yield();
typedef uint16_t pid_t;

void yield(void);
// uint8_t fork(void);
uint8_t rank(void);
pid_t pid(void);
pid_t spawn(void (*fn)(void), uint8_t const priority);
void set_priority(uint8_t const priority);
void kill_curr_task(void);

void wait_for_task_finnish(pid_t);

void terminate(void);

#define print(fmt, ...)               \
  asm volatile("cli");                \
  printf_P(PSTR(fmt), ##__VA_ARGS__); \
  asm volatile("sei");

#define HALT_EXEC()                 \
  print("Halting!");                \
  asm volatile("cli" ::: "memory"); \
  for (;;)                          \
    ;

#endif  // OS_H_