#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#include "interupts.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos.h"

volatile Scheduler simplos_schedule;

// Extern global variable to modify the stack pointer using macros from
// simplos.h

volatile uint16_t _task_sp_adr = 0;
volatile uint16_t* volatile task_sp = &_task_sp_adr;

bool debug_queu_is_initialised = false;

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
  sei();

  // disable timer compare interrupt for now
  DISABLE_MT();

  dprint("Starting!\n");
  dprint("Starting2!\n");
  dprint("Starting3!\n");

  dprint("hello world!\n");

  init_empty_queue(&simplos_schedule.queue);

  uint8_t const index = add_task_to_queue(0, &simplos_schedule.queue);
  Simplos_Task* new_task = &simplos_schedule.queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->empty = false;
  simplos_schedule.queue.curr_task_index = index;

  // Jump to the new task.

  *task_sp = simplos_schedule.queue.task_queue[index].task_sp_adr;
  // Run idle function. Should never leave this.
  SET_SP();
  // asm volatile("nop");
  idle_fn(&simplos_schedule);
  fatal_error("UNREACHABLE END OF MAIN");

  // Should not be reached!
  dprint("Should not happen!!!!\n");
  for (;;) {
    dprint("This is very odd\n");
  }
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  context_switch();
  asm volatile("reti");
}
