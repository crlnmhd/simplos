#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"

// TODO better scheduling. This is just a placeholder.

NO_MT void static_cyclic_scheduler(void) {
  // This defaults to the idle task.
  // print_schedule(kernel->schedule);

  // cprint("At block: %d\n", kernel->schedule.queue.curr_task_index);
  for (uint8_t i = 1; i <= TASKS_MAX; ++i) {
    uint8_t const next_candidate_index =
        ((uint8_t)(INDEX_OF_CURRENT_TASK + i) % TASKS_MAX);

    if (kernel->schedule.queue.tasks[next_candidate_index].status == READY) {
      INDEX_OF_CURRENT_TASK = next_candidate_index;
      cprint(
          "selected task  %d \"%s\" with SP=0x%X using circular scheduling.\n",
          INDEX_OF_CURRENT_TASK, kernel->task_names[INDEX_OF_CURRENT_TASK],
          kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].task_sp_adr);
      break;
    }
  }
  if (kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].status == EMPTY) {
    fatal_error("No task ready to run!\n");
  }
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
      if (remaining_tasks_to_schedule == 0) {
        return;
      } else {
        const uint8_t task_list_index = unsorted[task_index];
        if (kernel->schedule.queue.tasks[task_list_index].priority ==
            priority) {
          kernel->schedule.queue.task_index_queue[sorted_task_counter] =
              task_list_index;
          sorted_task_counter++;
        }
      }
    }
  }
}

void prioity_scheduler(void) {
  ASSERT(false, "Priority schduling is not implemented!\n");
}

void select_next_task(void) {
#if defined(PRIORITY_SCHEDULING)
  prioity_scheduler();
#else
  static_cyclic_scheduler();
#endif
}
