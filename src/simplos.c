#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "tasks.h"

uint16_t task_default_sp(uint8_t task_memory_block) {
  return RAMSTART + task_memory_block * TASK_MEMORY_BYTES;
}

void init_empty_queue(volatile Task_Queue* queue) {
  // queue->curr_new_task_index = 0;
  // Simplos_Task* idle_task = queue->task_queue;
  // // Idle task
  // idle_task->empty = false;
  // idle_task->fn = idle_task_fn;
  // idle_task->priority = 0;
  // idle_task->status = READY;
  // idle_task->task_memory_block = 0;
  // idle_task->task_sp = RAMSTART;
  // // TODO run idle task?
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    task->empty = true;
    task->task_memory_block = i;
    task->task_sp = task_default_sp(task->task_memory_block);
  }
}
uint8_t add_task_to_queue(uint8_t priority, volatile Task_Queue* queue) {
  DISABLE_MT();
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    // Take if available
    if (task->empty) {
      printf("Initiating space for new function at block %d\n", i);
      task->empty = false;
      task->priority = priority;
      task->status = READY;
      ENABLE_MT();
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");
  for (;;) {
    printf("Still an error...\n");
  }
  // Never reached.
  return 0;
}

void create_task(void (*fn)(void), uint8_t priority,
                 volatile Scheduler* schedule) {
  // New task
  uint8_t const new_task_index =
      add_task_to_queue(priority, &((Scheduler*)schedule)->queue);
  volatile Simplos_Task* new_task = &schedule->queue.task_queue[new_task_index];
  // Just to be sure this does not mess things up.
  SAVE_SP();
  uint16_t const old_task_sp = task_sp;

  // Yield to let the context switch happen.
  printf("yielding...\n");
  yield();
  DISABLE_MT();

  printf("This is after yielding.\n");
  if (schedule->queue.curr_task_index == new_task_index) {
    // Run function
    printf("Running new function %p at block %d\n", fn, new_task_index);
    // Give the function the maximum possible time to run by setting interupt
    // counter to 0.
    TCNT1 = 0;

    ENABLE_MT();
    fn();
    DISABLE_MT();
    printf("Task %ul done!\n", new_task_index);
    kill_task(schedule, new_task_index);
  } else {
    printf("Returning to caller.\n");
    task_sp = old_task_sp;
    SET_SP();
    // FIXME do things here!
  }
  ENABLE_MT();
}

void kill_task(volatile Scheduler* schedule, uint8_t task) {
  DISABLE_MT();
  printf("killing task: %d\n", task);
  Simplos_Task* victim = (Simplos_Task*)&schedule->queue.task_queue[task];
  victim->status = DONE;
  victim->empty = true;
  victim->task_sp = task_default_sp(task);
}
