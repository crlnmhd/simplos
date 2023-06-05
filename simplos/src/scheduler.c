#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"
#include "simplos_types.h"
#include "timers.h"

uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks);
void assign_scheduler(uint8_t *task_block_list, const uint8_t starting_index,
                      const uint8_t end_index);
void prioritize_tasks(taskptr_type tasks, const uint8_t num_tasks,
                      volatile uint8_t *out_priority_list);

void print_queue(uint8_t num_active_tasks);

void reschedule(void) {
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING();
  const uint8_t num_active_tasks =
      get_active_tasks(kernel->schedule.queue.task_index_queue, TASKS_MAX);
  END_DISCARD_VOLATILE_QUALIFIER_WARNING();
  if (num_active_tasks == 0) {
    FATAL_ERROR("Error, no tasks avalable to schedule!");
  }

  prioritize_tasks(kernel->schedule.queue.tasks, num_active_tasks,
                   kernel->schedule.queue.task_index_queue);
  kernel->schedule.queue.queue_position = (uint8_t)(num_active_tasks - 1);
#if defined(VERBOSE_OUTPUT)
  print_queue(num_active_tasks);
#endif  // defined(VERBOSE_OUTPUT)
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
 * Sets the priority order of the tasks queue in out_priority_list.
 * In decreasing order of priority.
 * */
void prioritize_tasks(taskptr_type volatile tasks,
                      const uint8_t num_tasks_in_queue,
                      volatile uint8_t *out_priority_list) {
  uint8_t num_remaining = num_tasks_in_queue;
  for (uint16_t priority = 0; priority < UINT8_MAX; priority++) {
    for (uint8_t i = 0; num_remaining > 0 && i < num_tasks_in_queue; i++) {
      if (tasks[i].priority == priority) {
        const int sort_index = num_tasks_in_queue - num_remaining;
        out_priority_list[sort_index] = i;
        num_remaining--;
      }
    }
  }
}

void print_queue(uint8_t num_active_tasks) {
  ASSERT(num_active_tasks < TASKS_MAX,
         "Invalid number of active tasks provided!");

  cprint("Start of queue:\n-------------\n");
  for (uint8_t i = 0; i < num_active_tasks; i++) {
    cprint("%d: ", i);
    uint8_t task_index = kernel->schedule.queue.task_index_queue[i];
    taskptr_type task_ptr = &kernel->schedule.queue.tasks[task_index];
    print_task(task_ptr);
  }
  cprint("End of queue:\n-------------\n");
}

void select_next_task(void) {
  if (kernel->schedule.queue.queue_position == 0) {
#if defined(VERBOSE_OUTPUT)
    cprint("Rescheduling...\n");
#endif  // defined(VERBOSE_OUTPUT)
    reschedule();
  } else {
    kernel->schedule.queue.queue_position--;
  }
  next_task_sp = INDEX_OF_CURRENT_TASK;
}
void schedule_tasks(void) { reschedule(); }

void start_scheduler(void) {
  cprint("Scheduler started. Yielding...\n");
  k_yield();  // Contect swtich back after starting
  while (true) {
#if defined(VERBOSE_OUTPUT)
    cprint("Selecting next task...\n");
#endif  // defined (VERBOSE_OUTPUT)
    select_next_task();
    k_yield();
  }
}
