#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"

// TODO stack check function.

void init_empty_queue(Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    task->task_memory_block = i;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    cprint("Initiating mem block %d with SP 0x%X\n", i, task->task_sp_adr);
    task->status = EMPTY;
  }
}

// NOT TS
uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    // Take if available
    if (task->status == EMPTY) {
      cprint("Initiating space for new function at block %d\n", i);
      task->priority = priority;
      task->status = READY;
      task->task_sp_adr = task_default_sp(task->task_memory_block);
      return i;
    }
  }
  fatal_error("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

// void kill_task(uint8_t const task, bool reinable_mt) {
//   DISABLE_MT();
//   cprint("killing task: %d\n", task);
//   Simplos_Task* victim = &simplos_schedule->queue.task_queue[task];
//   cprint("Murder victim:");
//   victim->status = EMPTY;
//   print_task(victim, true);
//   victim->task_sp_adr = task_default_sp(task);
//   cprint("Task %d killed\n", task);
//   if (reinable_mt) {
//     ENABLE_MT();
//   }
// }

// void kill_current_task() {
//   DISABLE_MT();
//   uint8_t const curr_task_index = simplos_schedule->queue.curr_task_index;
//   kill_task(curr_task_index, false);
//   // k_yield();  // enables MT.
//   uint8_t const task_nr = select_next_task();  // Updates curr_task_index
//   Simplos_Task* new_task = &simplos_schedule->queue.task_queue[task_nr];
//   prepare_next_task(new_task);
// }

void k_yield(void) {
  cli();
  context_switch();
  TCNT1 = 0;
  sei();
  asm volatile("ret" ::);
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  context_switch();
  reti();
}

// void kill_current_task(void) {
//   ENABLE_MT();
//   cli();
//   simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
//       .status = EMPTY;
//   k_yield();  // reinables interupts.
// }