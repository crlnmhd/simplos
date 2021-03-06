#include "simplos.h"
#include "serial.h"

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>

#define USE_STATIC 1

void select_next_task(Scheduler* scheduler)
{
  // This defaults to the idle task.

  if (scheduler->force_prev)
  {
    printf("Forcing previous\n");
    scheduler->queue.curr_task_index = scheduler->prev_task;
    scheduler->force_prev = false;
    return;
  }

#ifdef USE_STATIC
  scheduler->prev_task = scheduler->queue.curr_task_index;
  scheduler->force_prev = false;
  for (uint8_t i = 1; i < TASKS_MAX; ++i)
  {
    uint8_t const next_candidate_index = scheduler->queue.curr_task_index + i % TASKS_MAX;
    if (scheduler->queue.task_queue[next_candidate_index].status == READY)
    {
      scheduler->queue.curr_task_index = next_candidate_index;
      printf("selected task  %d using circular scheduling.\n", next_candidate_index);

      break;
    }
  }
  printf("Thats odd!\n");

#else
  uint8_t next_to_run_index = 0;
  uint8_t highest_priority = 0;
  Simplos_Task* t = scheduler->queue.task_queue;
  for (uint8_t i = 0; i < TASKS_MAX; ++i, ++t)
  {
    // printf("Task %d status: %u\n", i, t->status);
    if (!t->empty)
    {
      if (t->status == READY && t->priority > highest_priority)
      {
        highest_priority = t->priority;
        next_to_run_index = i;
      }
    }
  }
  scheduler->prev_task = scheduler->queue.curr_task_index;
  scheduler->force_prev = false;
  scheduler->queue.curr_task_index = next_to_run_index;
#endif // USE STATIC
}
