#ifdef MOCK_HAL
#include <stdint.h>

#include "hal.hpp"

constexpr uint16_t log_buf_size{512};
char log_buf[512] = {};

Log hal_log{log_buf, log_buf_size};

/*
 * Mock hall used for testing
 * */
HardwareState state = {.interrupt_enabled = false, .halted = false};

void halt_exec(void) { state.halted = true; }
void enable_interrupts(void) { state.interrupt_enabled = true; }
void disable_interrupts(void) { state.interrupt_enabled = false; }

/* Placeholders*/
void write_mm(uint8_t *const, const uint8_t) {}
uint8_t read_mm(uint8_t *const) { return 0xFF; }
uint8_t read_from_mm_adr(const uint16_t) { return 0xFF; }
void printf_flash(const char *, ...) {}
void setup_interupt_timer(const uint16_t) {}
void init_measurement_ticks(void) {}
uint16_t get_tick_counter(void) { return 0xFF; }
void clear_tick_counter(void) {}

void mocked_fatal_error(const ProgmemString &message) {
  hal_log.add_entry(message);
}

#endif  // MOCK_HAL
