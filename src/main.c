#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "interupts.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos.h"

Scheduler volatile _simplos_schedule;
Scheduler volatile* volatile simplos_schedule = &_simplos_schedule;

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile uint16_t _task_sp_adr = 0;
volatile uint16_t* volatile task_sp = &_task_sp_adr;
uint16_t volatile pid_cnt = 0;

int main(void) {
  // Initialite serial communication.
  uart_init();
  FILE uart_file =
      FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  // FIXME this hides a bug? where some characters from the first serial message
  // are repeated in the begining.
  puts("");

  init_timer_interupts();
  cli();

  // disable timer compare interrupt for now
  DISABLE_MT();

  cprint("Starting1!\n");
  cprint("Starting2!\n");
  cprint("Starting3!\n");

  init_schedule();

  uint8_t const index = add_task_to_queue(0, &simplos_schedule->queue);
  Simplos_Task* new_task = &simplos_schedule->queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->pid = pid_cnt++;
  simplos_schedule->queue.curr_task_index = index;

  // Jump to the new task.

  *task_sp = simplos_schedule->queue.task_queue[index].task_sp_adr;
  // Run idle function. Should never leave this.
  SET_SP();
  // asm volatile("nop");
  sei();
  ENABLE_MT();
  idle_fn();
  fatal_error("UNREACHABLE END OF MAIN");

  // Should not be reached!
  cprint("Should not happen!!!!\n");
  for (;;) {
    cprint("This is very odd\n");
  }
}
