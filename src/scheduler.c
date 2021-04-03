#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include "serial.h"
#include "simplos.h"

#define USE_STATIC

volatile Simplos_Task* select_next_task(volatile Scheduler* scheduler) {
  // This defaults to the idle task.
  print_tasks(scheduler);

  // if (scheduler->force_prev)
  // {
  //   printf("Forcing previous\n");
  //   scheduler->queue.curr_task_index = scheduler->prev_task;
  //   scheduler->force_prev = false;
  //   return;
  // }

#ifdef USE_STATIC
  scheduler->prev_task = scheduler->queue.curr_task_index;
  scheduler->force_prev = false;
  printf("At block: %d\n", scheduler->queue.curr_task_index);
  for (uint8_t i = 1; i <= TASKS_MAX; ++i) {
    uint8_t const next_candidate_index =
        (scheduler->queue.curr_task_index + i) % TASKS_MAX;
    if (scheduler->queue.task_queue[next_candidate_index].status == READY) {
      scheduler->queue.curr_task_index = next_candidate_index;
      printf("selected task  %d using circular scheduling.\n",
             next_candidate_index);
      break;
    }
  }
#else
  uint8_t next_to_run_index = 0;
  uint8_t highest_priority = 0;
  Simplos_Task* t = scheduler->queue.task_queue;
  for (uint8_t i = 0; i < TASKS_MAX; ++i, ++t) {
    // printf("Task %d status: %u\n", i, t->status);
    if (!t->empty) {
      if (t->status == READY && t->priority > highest_priority) {
        highest_priority = t->priority;
        next_to_run_index = i;
      }
    }
  }
  scheduler->prev_task = scheduler->queue.curr_task_index;
  scheduler->force_prev = false;
  scheduler->queue.curr_task_index = next_to_run_index;
#endif  // USE STATIC

  return &scheduler->queue.task_queue[scheduler->queue.curr_task_index];
}

void print_tasks(volatile Scheduler* scheduler) {
  DISABLE_MT();
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    volatile Simplos_Task* task = &scheduler->queue.task_queue[i];
    printf("Task at block %d : ", i);
    switch (task->status) {
      case READY:
        printf("READY\n");
        break;
      case DONE:
        printf("DONE\n");
        break;
      case RUNNING:
        printf("RUNNING\n");
        break;
      default:
        printf("OTHER\n");
        break;
    }
  }
  ENABLE_MT();
}
