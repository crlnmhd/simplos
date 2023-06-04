#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "memory_layout.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos.h"
#include "tests/test.h"
#include "timers.h"

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile Kernel internal_kernel_location
    __attribute((section(".kernel_data_location")));
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t task_sp __attribute__((section(".task_sp_location"))) = 0;

int main(void) {
  cli();
  SP = INITIAL_STACK_START;

  /* Set up serial communication */
  uart_init();

  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  pre_scheduler_self_checks();

  /* Set up  timer interrupt for tick counting. */
  init_scheduler_interupts();

  // disable timer compare interrupt for now
  DISABLE_MT();

  /* Set up task memory regions, including canaries */
  init_memory();

  init_kernel();
  init_schedule();
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t index = create_simplos_task("idle_fn", 0);
  INDEX_OF_CURRENT_TASK = index;

  const uint8_t margin_to_main = 10;

  configure_heap_location(margin_to_main);
  task_sp = kernel->schedule.queue.tasks[index].task_sp_adr;
  SET_SP();

  /* Run idle function. Should never leave this. */
  sei();
  ENABLE_MT();
  run_idle_fn();
  FATAL_ERROR("UNREACHABLE END OF MAIN\n");
}
