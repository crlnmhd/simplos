#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"
// #include "tasks.h"

// TODO stack check function.

extern bool debug_queu_is_initialised;
void init_empty_queue(Task_Queue* queue)

{
  if (debug_queu_is_initialised) {
    fatal_error("QUEUE APPEARS TO ALREADY BE INITIALISED!");
  }

  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    task->empty = true;
    task->task_memory_block = i;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    task->spawning = false;
  }
  debug_queu_is_initialised = true;
}

// NOT TS
uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    // Take if available
    if (task->empty) {
      dprint("Initiating space for new function at block %d\n", i);
      task->empty = false;
      task->priority = priority;
      task->status = READY;
      return i;
    }
  }
  fatal_error("Can't create new task! Queue if full!");
  for (;;) {
    printf("Still an error...\n");
  }
  // Never reached.
  return 0;
}

void kill_task(Scheduler* schedule, uint8_t task) {
  DISABLE_MT();
  dprint("killing task: %d\n", task);
  Simplos_Task* victim = (Simplos_Task*)&schedule->queue.task_queue[task];
  victim->status = DONE;
  victim->empty = true;
  victim->task_sp_adr = task_default_sp(task);
}

void kill_current_task(Scheduler* schedule) {
  uint8_t const curr_task_index = schedule->queue.curr_task_index;
  kill_task(schedule, curr_task_index);
  yield();
}