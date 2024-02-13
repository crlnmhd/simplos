#ifndef ATMEGA2560_HAL_HPP_
#define ATMEGA2560_HAL_HPP_

#define NORETURN __attribute__((noreturn))

NORETURN void halt_exec(void);

static inline __attribute__((always_inline)) void disable_interrupts(void) {
  asm volatile("cli" ::: "memory");
}
static inline __attribute__((always_inline)) void enable_interrupts(void) {
  asm volatile("sei" ::: "memory");
}
#endif  // ATMEGA2560_HAL_HPP_
