#ifndef MOCK_HAL_HPP_
#define MOCK_HAL_HPP_

#define NORETURN

#include "log.hpp"
// TODO: save list of called commands?
struct HardwareState {
  bool interrupt_enabled;
  bool halted;
};

extern HardwareState state;

void halt_exec(void);
void disable_interrupts(void);
void enable_interrupts(void);

#endif  // MOCK_HAL_HPP_
