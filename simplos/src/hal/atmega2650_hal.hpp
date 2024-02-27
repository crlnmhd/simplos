#ifndef ATMEGA2560_HAL_HPP_
#define ATMEGA2560_HAL_HPP_

#ifdef MOCH_HAL
#error "Mocked hal must not be enabled together with real hal header"
#endif

#define NORETURN __attribute__((noreturn))

NORETURN void halt_exec(void);

static inline __attribute__((always_inline)) void disable_interrupts(void) {
  asm volatile("cli" ::: "memory");
}
static inline __attribute__((always_inline)) void enable_interrupts(void) {
  asm volatile("sei" ::: "memory");
}

#define FATAL_ERROR(message, ...)               \
  disable_interrupts();                         \
  print_from_prg_mem("FATAL ERROR!\n");         \
  print_from_prg_mem((message), ##__VA_ARGS__); \
  print_from_prg_mem(("\n"));                   \
  halt_exec();

#endif  // ATMEGA2560_HAL_HPP_
