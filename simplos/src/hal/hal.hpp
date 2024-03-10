#ifndef HAL_H_
#define HAL_H_

#include <stdbool.h>
#include <stdio.h>

#ifdef MOCK_HAL
#include "mock_hal.hpp"
#else
#include "atmega2650_hal.hpp"
#endif

extern volatile uint16_t task_sp;
extern volatile uint16_t next_task_sp;
extern volatile uint16_t prev_task_sp;
extern volatile uint16_t scheduler_task_sp;

void write_mm(uint8_t *const mem_ptr, const uint8_t value);
uint8_t read_mm(uint8_t *const mem_ptr);
uint8_t read_from_mm_adr(const uint16_t mem_adr);

void printf_flash(const char *fmt, ...);

void setup_interupt_timer(const uint16_t frequenzy_hz);
void init_measurement_ticks(void);

uint16_t get_tick_counter(void);  // TODO: inline mem access for performance?
void clear_tick_counter(void);

void uart_init(void);
int16_t uart_putchar(char c, FILE *);
int16_t uart_getchar(FILE *);

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5

#endif  // HAL_H_
