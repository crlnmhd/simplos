#if !defined(OS_H_)
#define OS_H_

#if defined(__clang__)
// clang-format off
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif

#include <avr/pgmspace.h>
#include <stdint.h>
#include "progmem.hpp"

typedef uint16_t pid_t;

// clang-format on

void yield(void);
// uint8_t fork(void);
uint8_t rank(void);
pid_t pid(void);
pid_t spawn(void (*fn)(void), uint8_t const priority,
            const ProgmemString &name);
void set_priority(uint8_t const priority);

// Extern C for access from asm. Otherwise the name would be mangled.
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
void kill_curr_task(void);
#ifdef __cplusplus
}
#endif  // __cplusplus

void wait_for_task_finnish(pid_t);

void terminate(void);
/*
 * Attempts to allocate 'bytes' of continuous memory. Returns nullptr on
 * failure.
 * */
void *malloc(size_t bytes);

// mutex* mutex_create(void);
// void mutex_lock(mutex* mtx);
// void mutex_unlock(mutex* mtx);

#define print(fmt, ...) printf_flash(PSTR(fmt), ##__VA_ARGS__)

void halt(void);

#ifdef MOCK_HAL
#include "simplos_types.hpp"
void set_os_kernel(Kernel *kernel);
#endif  // MOCK_HAL

#endif  // OS_H_
