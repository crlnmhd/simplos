#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "scheduler.h"
#include "serial.h"
#include "simplos.h"

volatile Scheduler simplos_schedule;

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile uint16_t task_sp = 0;

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

  // disable timer compare interrupt for now
  DISABLE_MT();

  printf("Starting!\n");
  printf("Starting2!\n");
  printf("Starting3!\n");

  init_empty_queue(&simplos_schedule.queue);

  uint8_t const index = add_task_to_queue(0, &simplos_schedule.queue);
  volatile Simplos_Task* new_task = &simplos_schedule.queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->empty = false;
  simplos_schedule.queue.curr_task_index = index;

  // Jump to the new task.
  task_sp = (uint16_t)simplos_schedule.queue.task_queue[index].task_sp;

  printf("Main setting stack pointer to that of task %d: %u\n", index, task_sp);

  SET_SP();
  PUSH_PC();
  printf("hi!\n");
  // Run idle function. Should never leave this.

  idle_fn(&simplos_schedule);

  // Should not be reached!
  printf("Should not happen!!!!\n");
  for (;;) {
    printf("This is very odd\n");
  }
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) { context_switch(); }