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

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5

static Scheduler schedule;

// Extern global variable to update SP
volatile uint16_t task_sp = 0;

int main()
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
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  schedule.enabled = false;
  printf("Starting!\n");
  printf("Starting2!\n");
  printf("Starting3!\n");

  init_empty_queue(&schedule.queue);
  // SAVE_CONTEXT();
  // main_sp = *STACK_POINTER;
  // task_sp = *STACK_POINTER;

  schedule.enabled = true;
  setup_idle_task(idle_fn, &schedule);

  // FIXME Can't create more tasks after this.
  // ENABLE_MT_IDLE_TASK(idle_task);
  
  // Should not reach this
  ENABLE_MT();
  printf("Should not happen!!!!\n");
  for (;;);

  // debug_print_message(&test[0], &test[99]);
}


// Here for easy access to global static variable.
ISR(TIMER1_COMPA_vect)
{
  printf("Context switch!\n");
  // fflush(stdout);

  INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  // printf("hi!\n");

  SAVE_CONTEXT();
  SAVE_SP();
  // fflush(stdout);


  Simplos_Task* prev = &schedule.queue.task_queue[schedule.queue.curr_task_index];
  prev->task_sp = task_sp;

  if (schedule.enabled)
  {
    printf("Using scheduler\n");
    // prev->status = READY;
    // printf("Equal? prev: %u =?= %u : schedule\n", prev->task_sp, schedule.queue.task_queue[schedule.queue.curr_task_index].task_sp);
    select_next_task(&schedule);   // Updates curr_task_index
    //printf("Context switch from %d to %d\n", prev->task_memory_block, schedule.queue.curr_task_index);
    // Save old stack pointer to the previous task
    // Set SP for the new task.
    task_sp = schedule.queue.task_queue[schedule.queue.curr_task_index].task_sp;
  }
  // // Return to the main() function to add more tasks.
  // else
  // {
  //   task_sp = main_sp;
  //   printf("Returning to main()\n");
  // }

  SET_SP();
  RESTORE_CONTEXT();
  INTERNAL_LED_PORT &= ~(1 << INTERNAL_LED);
}