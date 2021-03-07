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
    Simplos_Task* task = &queue->task_queue[i];
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

void create_task(void (*fn)(void), uint8_t priority, volatile Scheduler* schedule)
{
  DISABLE_MT();

  //printf("Initiating new function!\n");

  uint8_t const old_task_index = schedule->queue.curr_task_index;
  schedule->prev_task = old_task_index;

  volatile Simplos_Task* old_task = &schedule->queue.task_queue[old_task_index];
  old_task->status = READY;

  SAVE_SP();
  old_task->task_sp = task_sp;

  // New task
  uint8_t const new_task_index = add_task_to_queue(priority, &((Scheduler*)schedule)->queue);

  schedule->queue.curr_task_index = new_task_index;
  Simplos_Task* new_task = &((Scheduler*)schedule)->queue.task_queue[new_task_index];
  schedule->enabled = true;
  ENABLE_MT();
  fn();
  DISABLE_MT();
  // function completed!
  new_task->status = DONE;
  printf("Task done! Yielding...\n");
  yield();
  printf("curr task index: %d, old: %d\n", schedule->queue.curr_task_index, old_task_index);
  if(schedule->queue.curr_task_index == old_task_index)
  {
    printf("Attempting to return to calling function.\n");
    //change_task_sp(old_task_index, (Scheduler*)schedule);
    //RESTORE_CONTEXT();
    return;
  }

  printf("Waiting for task switch...\n");

  for (;;)
  {
    printf("%d", schedule->queue.curr_task_index);
    fflush(stdout);
  }
  return;
}

// Timer interupt 

