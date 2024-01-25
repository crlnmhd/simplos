#include "scheduler.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "hal/context_switch_macros.h"
#include "hal/hal.h"
#include "io_helpers.h"
#include "simplos.h"
#include "simplos_types.h"
#include "timers.h"

void assert_stack_pointer_points_to_valid_return_address(
    uint16_t adr_of_saved_task);
void handle_previous_task(Simplos_Task *prev, Kernel *kernel);
void prepare_next_task(Simplos_Task *next, Kernel *kernel);
void print_queue(uint8_t num_active_tasks, Kernel *kernel);

void reschedule(Kernel *kernel) {
  const uint8_t num_active_tasks = prioritize_tasks(
      kernel->schedule.queue.tasks, kernel->schedule.queue.task_index_queue);
  if (num_active_tasks == 0) {
    FATAL_ERROR("Error, no tasks avalable to schedule!");
  }

  kernel->schedule.queue.queue_position = (uint8_t)(num_active_tasks - 1);

#if defined(VERBOSE_OUTPUT)
  print_queue(num_active_tasks, kernel);
#endif  // defined(VERBOSE_OUTPUT)
}

uint8_t prioritize_tasks(Simplos_Task *tasks, uint8_t *out_priority_list) {
  uint8_t num_sored = 0;
  for (uint8_t priority = 0; priority < (uint8_t)UINT8_MAX; priority++) {
    for (uint8_t i = 0; i < TASKS_MAX; i++) {
      const bool task_is_active = tasks[i].status == Task_Status::READY;
      if (task_is_active && tasks[i].priority == priority) {
        out_priority_list[num_sored] = i;
        num_sored++;
      }
    }
  }
  return num_sored;
}

void print_queue(uint8_t num_active_tasks, Kernel *kernel) {
  debug_print("Start of queue:\n-------------\n");
  for (uint8_t i = 0; i < num_active_tasks; i++) {
    debug_print("%u: ", i);
    uint8_t task_index = kernel->schedule.queue.task_index_queue[i];
    Simplos_Task *task_ptr = &kernel->schedule.queue.tasks[task_index];
    print_task(task_ptr, kernel);
  }
  debug_print("End of queue:\n-------------\n");
}

uint8_t select_next_task(Kernel *kernel_ptr) {
  if (kernel_ptr->schedule.queue.queue_position == 0) {
    verbose_print("Rescheduling...\n");
    reschedule(kernel_ptr);
  } else {
    kernel_ptr->schedule.queue.queue_position--;
  }
  const uint8_t selected_task_index =
      kernel_ptr->schedule.queue
          .task_index_queue[kernel_ptr->schedule.queue.queue_position];

  next_task_sp =
      kernel_ptr->schedule.queue.tasks[selected_task_index].task_sp_adr;
  return selected_task_index;
}
void schedule_tasks(Kernel *kernel) { reschedule(kernel); }

#ifndef MOCK_HAL
void start_scheduler_with_os_kernel(void) {
  start_scheduler((Kernel *)global_kernel);
}  // Note: see header
#endif  // MOCK_HAL

void start_scheduler(Kernel *kernel) {
  SCILENT_DISABLE_MT();
  kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)].status =
      Task_Status::SCHEDULER;

  SET_RETURN_POINT(scheduler_loop_entry_point);
  SAVE_CONTEXT()
  SAVE_SP_TO_ADR(&scheduler_task_sp);
  while (true) {
    asm volatile("rjmp .+2");
  scheduler_loop_entry_point:
    asm volatile("nop");
    asm volatile("nop");
    SCILENT_DISABLE_MT();
    verbose_print("################## BEGIN SCHEDULING\n\n");
    verbose_print("Selecting next task...\n");
    Simplos_Task *prev =
        &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)];
    handle_previous_task(prev, kernel);
    const uint8_t new_task_index = select_next_task(kernel);

    kernel->schedule.active_task_block = new_task_index;

    Simplos_Task *next =
        &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)];
    verbose_print("Selected next task: \n");
#if defined(VERBOSE_OUTPUT)
    print_task(next, kernel);
#endif  // defined VERBOSE_OUTPUT
    prepare_next_task(next, kernel);
    verbose_print("Done selecting task. Next task sp = 0x%X\n", next_task_sp);
    verbose_print("################## END SCHEDULING\n\n");
    k_yield();  // enables MT
  }
}

void handle_previous_task(Simplos_Task *prev, Kernel *kernel) {
  if (prev->status == Task_Status::EMPTY) {
    return;  // Validation of task sp is not required since the task is empty.
  }

  if (prev->status == Task_Status::SCHEDULER) {
    prev->task_sp_adr = scheduler_task_sp;
    prev->status = Task_Status::SCHEDULER;
  } else if (prev->status == Task_Status::RUNNING) {
    prev->task_sp_adr = prev_task_sp;
    prev->status = Task_Status::READY;
  }
#if defined(VERBOSE_OUTPUT)
  print_schedule(kernel);
#endif  // defined (VERBOSE_OUTPUT)
  assert_task_pointer_integrity(prev, kernel);
  assert_stack_pointer_points_to_valid_return_address(prev->task_sp_adr);

  verbose_print("saving previous task %u's SP 0x%X\n", prev->task_memory_block,
                prev->task_sp_adr);
}
void assert_stack_pointer_points_to_valid_return_address(
    uint16_t adr_of_saved_task) {
  const uint8_t num_PC_bytes = 3;
  uint32_t task_PC = 0;
  const uint16_t on_stack_context_switch_overhead_after_saved_sp =
      num_context_switch_overhead_bytes() - num_PC_bytes -
      1;  // -1 from SP pointing to first free byte *after* the
          // content
  const uint16_t adr_of_stack_saved_pc =
      adr_of_saved_task + on_stack_context_switch_overhead_after_saved_sp;

  memcpy((void *)(&task_PC), (const uint16_t *)adr_of_stack_saved_pc,
         num_PC_bytes);  // Only copy the three bytes.

  const uint32_t invalid_bit_mask =
      0x00FE0000;  // PC is 17 bits wide, upper byte is never copied.

  ASSERT_EQ(task_PC & invalid_bit_mask, 0UL, "0x%lX",
            "Unexpected PC value found on task stack");

  ASSERT(task_PC != 0, "MALFORMED program couter for stack");
}

void prepare_next_task(Simplos_Task *next,
                       __attribute__((unused)) Kernel *kernel) {
  verbose_print("printing next:\n");
#if defined(VERBOSE_OUTPUT)
  print_task(next, kernel);
#endif  // defined VERBOSE_OUTPUT
  assert_task_pointer_integrity(next, kernel);
  if (next->status == Task_Status::EMPTY) {
    FATAL_ERROR("Error, can not select an empty task!\n");
  } else if (next->status == Task_Status::SCHEDULER) {
    FATAL_ERROR(
        "ERROR, Can not select scheduler task selected as next task!\n");
  } else {
    next->status = Task_Status::RUNNING;
  }
}

void invalidate_scheduled_queue(Kernel *kernel) {
  debug_print("Invalidating queue\n");
  kernel->schedule.queue.queue_position = 0;
}
