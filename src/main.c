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
  // The stack starts where the just before the .data section begins.
  SP = STACK_HIGH;
  // Initialite serial communication.
  uart_init();
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  cprint("space for kernel at 0x%X\n", &internal_kernel_location);
  init_timer_interupts();
  cli();

  // disable timer compare interrupt for now
  DISABLE_MT();
  init_memory();

  cprint("Kernel at  0x%X\n", &kernel);
  cprint("Scheduler at  0x%X\n", &simplos_schedule);
  cprint("Starting!\n");

  init_schedule();
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t const index = add_task_to_queue(0, &simplos_schedule.queue);
  Simplos_Task *new_task = &simplos_schedule.queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->pid = kernel->pid_cnt++;
  simplos_schedule.queue.curr_task_index = index;
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
  char *task_name_buf = kernel->task_names[index];
  END_DISCARD_VOLATILE_QUALIFIER_WARNING()
  strlcpy(task_name_buf, "test_fn", FUNCTION_NAME_MAX_LENGTH + 1);
  // Jump to the new task.
  kernel->heap_start = SP - 1;
  cprint("Heap starting at  0x%X\n", kernel->heap_start);
  ASSERT(kernel->heap_start > TASK_RAM_START,
         "init section has overflowed heap memory");
  cprint("%d bytes available for heap.\n", kernel->heap_start - TASK_RAM_START);

  *task_sp = simplos_schedule.queue.task_queue[index].task_sp_adr;
  SET_SP();

  // cprint("Kernel at at  0x%X\n", kernel);
  /* Run idle function. Should never leave this. */
  sei();
  ENABLE_MT();
#if defined(RUN_TESTS)
  run_tests();
#endif  // defined(RUN_TESTS)
  idle_fn();
  fatal_error("UNREACHABLE END OF MAIN");
}
