#ifndef OS_H_
#define OS_H_

#include <avr/pgmspace.h>
#include <stdint.h>

void yield(void);
// uint8_t fork(void);
uint8_t pid(void);
void spawn(void (*fn)(void), uint8_t const priority);
void set_priority(uint8_t const priority);
void kill_curr_task(void);

void terminate(void);

#define print(fmt, ...)               \
  asm volatile("cli");                \
  printf_P(PSTR(fmt), ##__VA_ARGS__); \
  asm volatile("sei");

#endif  // OS_H_