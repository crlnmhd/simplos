#include <avr/io.h>
#include <stdio.h>
//#include <util/delay.h>

#include "scheduler.h"
#include "serial.h"
#include "simplos.h"
#include "timers.h"

volatile Scheduler internal_simplos_schedule;
volatile Scheduler *volatile simplos_schedule = &internal_simplos_schedule;

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile uint16_t internal_task_sp_adr = 0;
volatile uint16_t *volatile task_sp = &internal_task_sp_adr;
volatile uint16_t pid_cnt = 0;

// External variables
// volatile Kernel kernel;

int main(void) {
  // Initialite serial communication.
  uart_init();
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  // &(kernel->heap_mapping) = HEAP_START + 1;

  init_timer_interupts();
  cli();

  // disable timer compare interrupt for now
  DISABLE_MT();
  // init_heap();

  cprint("Starting!\n");

  init_schedule();
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t const index = add_task_to_queue(0, &simplos_schedule->queue);
  Simplos_Task *new_task = &simplos_schedule->queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->pid = pid_cnt++;
  simplos_schedule->queue.curr_task_index = index;
  // Jump to the new task.
  cprint("At main: SP is 0x%X\n", SP);
  ASSERT(SP > HEAP_START, "main() has overflowed heap memory");
  *task_sp = simplos_schedule->queue.task_queue[index].task_sp_adr;
  cprint("OS SP is 0x%X.\nTask sp: 0x%X.\n", simplos_schedule->os_task_sp,
         *task_sp);
  SET_SP();

  /* Run idle function. Should never leave this. */
  sei();
  ENABLE_MT();
  idle_fn();
  fatal_error("UNREACHABLE END OF MAIN");
}
