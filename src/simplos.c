#include "simplos.h"
#include "memory.h"
#include "tasks.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void init_empty_queue(Task_Queue* queue)
{
  // queue->curr_task_index = 0;
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
    Simplos_Task * task = &queue->task_queue[i];
    task->empty = true;
    task->task_sp = RAMSTART + i * TASK_MEMORY_BYTES;
    task->task_memory_block = i;
  }
}
uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue)
{
  for (uint8_t i = 0; i < TASKS_MAX; ++i)
  {
    Simplos_Task* task = &queue->task_queue[i];
    printf("Trying %d\n", i);
    // Take if available
    if (task->empty)
    {
      printf("Initiating new function at block %d\n", i);
      task->empty = false;
      task->priority = priority;
      task->status = READY;
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");
  // Never reached.
  return 0;
}


void create_task(void (*fn)(void), uint8_t priority, Scheduler* scheduler)
{
  DISABLE_MT();
  //printf("Initiating new function!\n");

  // if (scheduler->processes == TASKS_MAX)
  // {
  //   printf("Error! Can't create new task! Queue if full!\n");
  //   return;
  // }
  uint8_t task_index = add_task_to_queue(priority, &scheduler->queue);

  scheduler->prev_task = scheduler->queue.curr_task_index;
  // Can't go back if there is nothing to go back to.
  scheduler->force_prev = false;
  scheduler->queue.curr_task_index = task_index;
  Simplos_Task* task = &scheduler->queue.task_queue[task_index];

  SAVE_CONTEXT();

  ENABLE_MT();
  fn();
  DISABLE_MT();
  // function completed!
  task->status = DONE;
  printf("Task done!\n");
  // task->empty = true;
  ENABLE_MT();
  return;
}

// Timer interupt 

