#include "simplos.h"
#include "tasks.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void init_empty_queue(volatile Task_Queue* queue)
{
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
  for (uint8_t i = 0; i < TASKS_MAX; ++i)
  {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    task->empty = true;
    task->task_sp = RAMSTART + i * TASK_MEMORY_BYTES;
    task->task_memory_block = i;
  }
}
uint8_t add_task_to_queue(uint8_t priority, volatile Task_Queue* queue)
{
  DISABLE_MT();
  for (uint8_t i = 0; i < TASKS_MAX; ++i)
  {
    Simplos_Task* task = (Simplos_Task *) &queue->task_queue[i];
    // Take if available
    if (task->empty)
    {
      printf("Initiating new function at block %d\n", i);
      task->empty = false;
      task->priority = priority;
      task->status = READY;
      ENABLE_MT();
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");
  // Never reached.
  return 0;
}

void create_task(void (*fn)(void), uint8_t priority, volatile Scheduler* schedule)
{
  DISABLE_MT();

  //printf("Initiating new function!\n");

  uint8_t const old_task_index = schedule->queue.curr_task_index;

  // New task
  uint8_t const new_task_index = add_task_to_queue(priority, &((Scheduler*)schedule)->queue);
  volatile Simplos_Task * new_task = &schedule->queue.task_queue[new_task_index];
  // Just to be sure this does not mess things up.
  uint16_t const old_task_sp = task_sp;
  SAVE_SP();
  new_task->task_sp = task_sp;
  task_sp = old_task_sp;

  // Yield to let the context switch happen.
  printf("yielding...\n");
  yield(schedule);
  DISABLE_MT();
  printf("This is after yielding.\n");

  if (schedule->queue.curr_task_index == new_task_index)
  {
    // Run function
    printf("Running new task function!\n");
    ENABLE_MT();
    fn();
    DISABLE_MT();
    new_task->status = DONE;
    printf("Task %ul done!\n", new_task_index);
  }
  else
  {
    printf("Returning to caller.\n");
  }
  ENABLE_MT();
}
