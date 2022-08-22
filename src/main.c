#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "memory_layout.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos.h"
#include "test.h"
#include "timers.h"

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile Kernel internal_kernel_location;
volatile Kernel *volatile kernel = &internal_kernel_location;

volatile uint16_t internal_task_sp_adr = 0;
volatile uint16_t *volatile task_sp = &internal_task_sp_adr;

int main(void) {
  SP = STACK_HIGH;  // The stack starts where the just before the .data section
                    // begins.
  cli();

  /* Set up serial communication */
  uart_init();

  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

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

  kernel->schedule.queue.curr_task_index = index;

  const uint8_t margin_to_main = 10;

  configure_heap_location(margin_to_main);

  *task_sp = kernel->schedule.queue.task_queue[index].task_sp_adr;
  SET_SP();

  // cprint("Kernel at at  0x%X\n", kernel);
  /* Run idle function. Should never leave this. */
  sei();
  ENABLE_MT();
  idle_fn();
  fatal_error("UNREACHABLE END OF MAIN\n");
}
