#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"

#define USE_STATIC

uint8_t select_next_task(Scheduler* scheduler) {
  // This defaults to the idle task.
  print_tasks(scheduler);

  dprint("At block: %d\n", scheduler->queue.curr_task_index);
  for (uint8_t i = 1; i <= TASKS_MAX; ++i) {
    uint8_t const next_candidate_index =
        (scheduler->queue.curr_task_index + i) % TASKS_MAX;

    if (scheduler->queue.task_queue[next_candidate_index].status == READY) {
      scheduler->queue.curr_task_index = next_candidate_index;
      dprint("selected task  %d (SP=0x%X)using circular scheduling.\n",
             scheduler->queue.curr_task_index,
             scheduler->queue.task_queue[scheduler->queue.curr_task_index]
                 .task_sp_adr);
      break;
    }
  }
  return scheduler->queue.curr_task_index;
}

void print_tasks(Scheduler* scheduler) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    volatile Simplos_Task* task = &scheduler->queue.task_queue[i];
    dprint("Task at block %d : ", i);
    print_task(task, true);
  }
}
