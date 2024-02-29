#ifndef MOCK_HAL_HPP_
#define MOCK_HAL_HPP_

#ifdef MOCK_HAL

#define NORETURN

#include "log.hpp"

// TODO: save list of called commands?
struct HardwareState {
  bool interrupt_enabled;
  bool halted;
};

extern Log hal_log;

extern HardwareState state;

void halt_exec(void);
void disable_interrupts(void);
void enable_interrupts(void);

void mocked_fatal_error(const ProgmemString &message);

#define FATAL_ERROR(message) \
  mocked_fatal_error(progmem_string("FATAL ERROR\n" message))

#endif  // MOCK_HAL
#endif  // MOCK_HAL_HPP_
