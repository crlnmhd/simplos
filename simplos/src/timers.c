#include "timers.h"

#include <stdio.h>

#include "hal/hal.h"
#include "io_helpers.h"

void init_scheduler_interupts(void) {
  cprint("Setting up interrupts\n");
  setup_interupt_timer(10);
}

#if defined(SW_TIME_MEASSREMENTS)
// Use 16 bit timer to count approximate execution time.
void init_ticks(void) {
  cprint("Setting up ticks\n");
  init_measurement_ticks();
}
#endif  // defined SW_TIME_MEASSREMENTS

#if defined(SW_TIME_MEASSREMENTS)
NO_MT void print_cs_timing_data(void) { cprint("TODO: print timers"); }
#endif  // defined SW_TIME_MEASSREMENTS
