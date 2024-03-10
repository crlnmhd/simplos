#include "timers.hpp"

#include <stdio.h>

#include "hal/hal.hpp"
#include "io_helpers.hpp"

void init_scheduler_interupts(void) {
  debug_print("Setting up interrupts\n");
  setup_interupt_timer(10);
}

#if defined(SW_TIME_MEASSREMENTS)
// Use 16 bit timer to count approximate execution time.
void init_ticks(void) {
  debug_print("Setting up ticks\n");
  init_measurement_ticks();
}
#endif  // defined SW_TIME_MEASSREMENTS

#if defined(SW_TIME_MEASSREMENTS)
#endif  // defined SW_TIME_MEASSREMENTS
