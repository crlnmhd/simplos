#ifndef MOCK_HAL_HPP_
#define MOCK_HAL_HPP_

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

#define FATAL_ERROR(str, ...) hal_log.add_entry("FATAL ERROR " str)

#endif  // MOCK_HAL_HPP_
