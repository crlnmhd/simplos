#ifndef HAL_H_
#define HAL_H_

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "hal/context_switch_macros.h"

extern volatile uint16_t task_sp;
extern volatile uint16_t next_task_sp;
extern volatile uint16_t scheduler_task_sp;

#ifdef MOCK_HAL  // FIXME: avoid if possible

// TODO: save list of called commands?
struct HardwareState {
  bool interrupt_enabled;
  bool halted;
};

extern struct HardwareState state;

void halt_exec(void);
void disable_interrupts(void);
void enable_interrupts(void);
#else
__attribute__((noreturn)) void halt_exec(void);

static inline __attribute__((always_inline)) void disable_interrupts(void) {
  asm volatile("cli" ::: "memory");
}
static inline __attribute__((always_inline)) void enable_interrupts(void) {
  asm volatile("sei" ::: "memory");
}
#endif  // MOCK_HAL

void write_mm(uint8_t *const mem_ptr, const uint8_t value);
uint8_t read_mm(uint8_t *const mem_ptr);
uint8_t read_from_mm_adr(const uint16_t mem_adr);

void printf_flash(const char *fmt, ...);

void setup_interupt_timer(const uint16_t frequenzy_hz);
void init_measurement_ticks(void);

uint16_t get_tick_counter(void);  // TODO: inline mem access for performance?
void clear_tick_counter(void);

void TIMER1_COMPA_vect(void);

void uart_init(void);
int16_t uart_putchar(char c, FILE *);
int16_t uart_getchar(FILE *);

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5
// Timer interupt for context switching
#endif  // HAL_H_
