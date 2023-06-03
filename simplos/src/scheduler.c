#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"
#include "timers.h"

uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks);
void assign_scheduler(uint8_t *task_block_list, const uint8_t starting_index,
                      const uint8_t end_index);
void prioritize_tasks(const uint8_t num_tasks);

void reschedule(void) {
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING();
  const uint8_t num_active_tasks =
      get_active_tasks(kernel->schedule.queue.task_index_queue, TASKS_MAX);
  END_DISCARD_VOLATILE_QUALIFIER_WARNING();

  prioritize_tasks(num_active_tasks);
}

/*
 * fills list tasks_block_list of length num_tasks unique active tasks.
 * Return value: the number of unique tasks.
 *
 * Note: any values of tasks_block_list[<return_value -1>] have no meening.
 * */
uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks) {
  uint8_t active_task_counter = 0;
  for (uint8_t i = 0; i < num_tasks; ++i) {
    if (kernel->schedule.queue.tasks[i].status == READY) {
      tasks_block_list[active_task_counter] = i;
      active_task_counter++;
    }
  }
  return active_task_counter;
}

/*
 * Reorder the task list positions [0, num_tasks) in acending order of
 * priority.
 * */
void prioritize_tasks(const uint8_t num_tasks_in_queue) {
  uint8_t unsorted[TASK_QUEUE_LENGTH];
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING();
  memcpy(unsorted, kernel->schedule.queue.task_index_queue, num_tasks_in_queue);
  memset(kernel->schedule.queue.task_index_queue, 0, num_tasks_in_queue);
  END_DISCARD_VOLATILE_QUALIFIER_WARNING();

  uint8_t sorted_task_counter = 0;

  for (uint16_t priority = 0; priority < UINT8_MAX; priority++) {
    for (uint8_t task_index = 0; task_index < num_tasks_in_queue;
         task_index++) {
      const uint16_t remaining_tasks_to_schedule =
          (uint8_t)(num_tasks_in_queue - sorted_task_counter);
      if (remaining_tasks_to_schedule > 0) {
        const uint8_t task_list_index = unsorted[task_index];
        if (kernel->schedule.queue.tasks[task_list_index].priority ==
            priority) {
          kernel->schedule.queue.task_index_queue[sorted_task_counter] =
              task_list_index;
          sorted_task_counter++;
        }
      } else {
        return;
      }
    }
  }
}

void select_next_task(void) {
  while (kernel->schedule.queue.queue_position == 0) {
    reschedule();
  }
  cprint("Scheduled tasks:");
  kernel->schedule.queue.queue_position--;
  INDEX_OF_CURRENT_TASK =
      kernel->schedule.queue
          .task_index_queue[kernel->schedule.queue.queue_position];
}
void schedule_tasks(void) { reschedule(); }

void start_scheduler(void) {
  cprint("Scheduler started. Yielding...\n");
  k_yield();  // Contect swtich back after starting
  while (true) {
    cprint("Selecting next task...\n");
    select_next_task();
    k_yield();
  }
}
