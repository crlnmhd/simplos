#include "serial.h"
#include "simplos.h"
#include "scheduler.h"

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

volatile uint16_t main_sp;

// setup IO buffers
// FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
// FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);


volatile Scheduler simplos_schedule;

// Extern global variable to update SP
volatile uint16_t task_sp = 0;

int main(void)
{

  // Initialite serial communication.
  uart_init();
  FILE uart_file = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
  stdout = stdin = &uart_file;

  // FIXME this hides a bug? where some characters from the first serial message are repeated in the begining.
  puts("");

  //set timer1 interrupt at 2 Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1 = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 7812;// = (16*10^6) / (2*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // disable timer compare interrupt for now
  DISABLE_MT();

  printf("Starting!\n");
  printf("Starting2!\n");
  printf("Starting3!\n");

  init_empty_queue(&simplos_schedule.queue);

  uint8_t index = add_task_to_queue(0, &simplos_schedule.queue);
  volatile Simplos_Task* new_task = &simplos_schedule.queue.task_queue[index];
  new_task->status = RUNNING;
  new_task->empty = false;
  simplos_schedule.queue.curr_task_index = index;

  // Jump to the new task.
  task_sp = (size_t)&simplos_schedule.queue.task_queue[index].task_sp;
  SET_SP();
  // Run idle function. Should never leave this.
  idle_fn(&simplos_schedule);

  // Should not br reached!
  printf("Should not happen!!!!\n");
  for (;;)
  {
    printf("This is very odd\n");
  }
}

ISR(TIMER1_COMPA_vect)
{
  context_switch();
}