#include "scheduler.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "hal/context_switch_macros.h"
#include "io_helpers.h"
#include "simplos.h"
#include "simplos_types.h"
#include "timers.h"

void assert_stack_pointer_points_to_valid_return_address(
    uint16_t adr_of_saved_task);
uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks,
                         Kernel *kernel);
void assign_scheduler(uint8_t *task_block_list, const uint8_t starting_index,
                      const uint8_t end_index);
void prioritize_tasks(taskptr_type tasks, const uint8_t num_tasks,
                      volatile uint8_t *out_priority_list);
void handle_previous_task(taskptr_type prev, Kernel *kernel);
void prepare_next_task(taskptr_type next, Kernel *kernel);

void print_queue(uint8_t num_active_tasks, Kernel *kernel);

void reschedule(Kernel *kernel) {
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING();
  const uint8_t num_active_tasks = get_active_tasks(
      kernel->schedule.queue.task_index_queue, TASKS_MAX, kernel);
  END_DISCARD_VOLATILE_QUALIFIER_WARNING();
  if (num_active_tasks == 0) {
    FATAL_ERROR("Error, no tasks avalable to schedule!");
  }

  prioritize_tasks(kernel->schedule.queue.tasks, num_active_tasks,
                   kernel->schedule.queue.task_index_queue);
  kernel->schedule.queue.queue_position = (uint8_t)(num_active_tasks - 1);
#if defined(VERBOSE_OUTPUT)
  print_queue(num_active_tasks, kernel);
#endif  // defined(VERBOSE_OUTPUT)
}

/*
 * fills list tasks_block_list of length num_tasks unique active tasks.
 * Return value: the number of unique tasks.
 *
 * Note: any values of tasks_block_list[<return_value -1>] have no meening.
 * */
uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks,
                         Kernel *kernel) {
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
        const uint8_t sort_index =
            (uint8_t)(num_tasks_in_queue - num_remaining);
        out_priority_list[sort_index] = i;
        num_remaining--;
      }
    }
  }
}

void print_queue(uint8_t num_active_tasks, Kernel *kernel) {
  ASSERT(num_active_tasks < TASKS_MAX,
         "Invalid number of active tasks provided!");

  cprint("Start of queue:\n-------------\n");
  for (uint8_t i = 0; i < num_active_tasks; i++) {
    cprint("%u: ", i);
    uint8_t task_index = kernel->schedule.queue.task_index_queue[i];
    taskptr_type task_ptr = &kernel->schedule.queue.tasks[task_index];
    print_task(task_ptr, kernel);
  }
  cprint("End of queue:\n-------------\n");
}

void select_next_task(Kernel *kernel_ptr) {
  if (kernel_ptr->schedule.queue.queue_position == 0) {
#if defined(VERBOSE_OUTPUT)
    cprint("Rescheduling...\n");
#endif  // defined(VERBOSE_OUTPUT)
    reschedule(kernel_ptr);
  } else {
    kernel_ptr->schedule.queue.queue_position--;
  }
  const uint16_t new_task_index =
      kernel_ptr->schedule.queue
          .task_index_queue[kernel_ptr->schedule.queue.queue_position];

  next_task_sp = kernel_ptr->schedule.queue.tasks[new_task_index].task_sp_adr;
}
void schedule_tasks(Kernel *kernel) { reschedule(kernel); }

#ifndef MOCK_HAL
void start_scheduler_with_os_kernel(void) {
  start_scheduler(global_kernel);
}  // Note: see header
#endif  // MOCK_HAL

void start_scheduler(Kernel *kernel) {
  SCILENT_DISABLE_MT();
  cprint("Scheduler started. Yielding...\n");
  scheduler_task_sp =
      SP - num_context_switch_overhead_bytes();  // FIXME: incrrect??

  while (true) {
    SCILENT_DISABLE_MT();
#if defined(VERBOSE_OUTPUT)
    cprint("Selecting next task...\n");
#endif  // defined (VERBOSE_OUTPUT)
    taskptr_type prev =
        &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)];
    handle_previous_task(prev, kernel);
    select_next_task(kernel);
    taskptr_type next =
        &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)];
    prepare_next_task(next, kernel);
#if defined(VERBOSE_OUTPUT)
    cprint("Done selecting task. Next task sp = 0x%X\n", next_task_sp);
#endif  // defined VERBOSE_OUTPUT
    cprint("DEBUG: yeilding\n");
    k_yield();  // enables MT
  }
}

void handle_previous_task(taskptr_type prev, Kernel *kernel) {
  cprint("printing task:\n");
  print_task(prev, kernel);
  assert_task_pointer_integrity(prev, kernel);

  if (prev->status == RUNNING) {
    // The previous task has been killed.
    prev->task_sp_adr = task_sp;
    prev->status = READY;
#if defined(VERBOSE_OUTPUT)
    print_schedule(kernel);
#endif  // defined (VERBOSE_OUTPUT)
    assert_task_pointer_integrity(prev, kernel);
    assert_stack_pointer_points_to_valid_return_address(prev->task_sp_adr);

#if defined(VERBOSE_OUTPUT)
    cprint("saving previous task %u's SP 0x%X\n", prev->task_memory_block,
           prev->task_sp_adr);
#endif
  }
}
void assert_stack_pointer_points_to_valid_return_address(
    uint16_t adr_of_saved_task) {
  const uint8_t num_PC_bytes = 3;
  uint32_t task_PC = 0;
  memcpy((void *)(&task_PC), (const uint16_t *)adr_of_saved_task,
         num_PC_bytes);  // Only copy the three bytes.

  const uint32_t in_valid_bit_mask =
      0x00FE0000;  // PC is 17 bits wide, upper byte is never copied.
  ASSERT_EQ(task_PC & in_valid_bit_mask, 0, "0X%X",
            "Unexpected PC value found on task stack");
}

void prepare_next_task(taskptr_type next, Kernel *kernel) {
#if defined(VERBOSE_OUTPUT)
  cprint("printing next:\n");
  print_task(next, kernel);
  assert_task_pointer_integrity(next, kernel);
#endif  // defined VERBOSE_OUTPUT
  next->status = RUNNING;
}
