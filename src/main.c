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
  sei();
  asm volatile("nop");
  cli();
  uart_init();
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  cprint("space for kernel at 0x%X\n", &internal_kernel_location);
  init_timer_interupts();

  // disable timer compare interrupt for now
  DISABLE_MT();
  init_memory();

  cprint("Kernel at  0x%X\n", &kernel);
  cprint("kernel end at  0x%X\n", &kernel->heap_start + sizeof(uint16_t));
  cprint("Starting!\n");

  init_kernel();
  init_schedule();
#if defined(SW_TIME_MEASSREMENTS)
  init_ticks();
#endif  // defined SW_TIME_MEASSREMENTS

  uint8_t const index = add_task_to_queue(0, &kernel->schedule.queue);
  Simplos_Task *new_task = &kernel->schedule.queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->pid = kernel->pid_cnt++;
  kernel->schedule.queue.curr_task_index = index;
  strlcpy(kernel->task_names[index], "idle_fn", FUNCTION_NAME_MAX_LENGTH + 1);
  // Jump to the new task.
  cprint("OS SP set to: 0x%X\n", kernel->schedule.os_task_sp);

  const uint8_t remaining_main_margin = 10;
  kernel->heap_start = SP - remaining_main_margin;
  cprint("Heap starting at  0x%X\n", kernel->heap_start);
  ASSERT(kernel->heap_start > TASK_RAM_START,
         "init section has overflowed heap memory");
  cprint("%d bytes available for heap.\n", kernel->heap_start - TASK_RAM_START);

  *task_sp = kernel->schedule.queue.task_queue[index].task_sp_adr;
  SET_SP();

  // cprint("Kernel at at  0x%X\n", kernel);
  /* Run idle function. Should never leave this. */
  sei();
  ENABLE_MT();
  idle_fn();
  fatal_error("UNREACHABLE END OF MAIN\n");
}
