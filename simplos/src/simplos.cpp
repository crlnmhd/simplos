#include "simplos.hpp"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hal/context_switch_macros.hpp"
#include "hal/hal.hpp"
#include "io_helpers.hpp"
#include "memory.hpp"
#include "memory_layout.hpp"
#include "os.hpp"
#include "scheduler.hpp"
#include "simplos_types.hpp"
#include "timers.hpp"

void init_task_list(Task_Queue *queue) {
  warn_if_task_memory_can_not_be_divided_evenly_between_tasks();
  for (uint8_t i = 0; i < tasks_max; ++i) {
    Simplos_Task *task = &queue->tasks[i];
    task->task_memory_block = i;
    task->time_counter = 0;
    task->task_sp_adr = task_sp_range_high(task->task_memory_block);
    debug_print("Initiating mem block %u at 0x%X-0x%X\n", i,
                task_sp_range_low(task->task_memory_block), task->task_sp_adr);
    task->status = Task_Status::EMPTY;
  }
}

uint8_t add_to_task_list(uint8_t priority, Task_Queue *queue) {
  for (uint8_t i = 0; i < tasks_max; ++i) {
    Simplos_Task *task = (Simplos_Task *)&queue->tasks[i];
    // Take if available
    if (task->status == Task_Status::EMPTY) {
      verbose_print("Initiating space for new function at block %u\n", i);
      task->priority = priority;
      task->status = Task_Status::READY;
      task->task_sp_adr = task_sp_range_high(task->task_memory_block);
      task->pid = UINT16_MAX;
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

void init_schedule(Kernel &kernel) {
  kernel.schedule.os_task_sp = os_stack_start;
  init_task_list(&(kernel.schedule.queue));
  kernel.cs_time_counter = 0;
  kernel.schedule.active_task_block = 0;
}

__attribute__((noinline, naked, optimize("-fno-defer-pop"))) void k_yield(
    void) {
  asm volatile("cli" ::: "memory");
  CONTEXT_SWTICH();
  asm volatile(
      "sei  \n\t"
      "ret  \n\t"
      : /* No outputs*/
      : /* No input*/
      : "memory");
}

void init_kernel(Kernel &kernel) {
  kernel.schedule.queue.queue_position = 0;
  for (uint8_t i = 0; i < tasks_max; i++) {
    // Empty task name.
    kernel.schedule.queue.tasks[i].set_name(progmem_string(""));

    // Set task RAM range.
    MemorySpan &task_stack_range = kernel.task_RAM_ranges[i];
    task_stack_range.high = task_sp_range_high(i);
    task_stack_range.low = task_sp_range_low(i);
  }
}

void verify_that_kernel_is_uninitilized(Kernel &kernel) {
  if (kernel.pid_cnt != 0) {
    FATAL_ERROR("Attempted to re-run startup process. Memory wrap-arround?");
  }
}

pid_type __attribute__((optimize("-fno-defer-pop")))
spawn_task(void (*fn)(void), uint8_t const priority, const ProgmemString &name,
           Kernel &kernel) {
  disable_interrupts();
  SCILENT_DISABLE_MT();
  uint8_t const new_task_index = create_simplos_task(name, priority, kernel);

  Simplos_Task &new_task = kernel.schedule.queue.tasks[new_task_index];
  new_task.status = Task_Status::RUNNING;

  uint16_t const new_task_pid = new_task.pid;

  Simplos_Task &old_task =
      kernel.schedule.queue.tasks[index_of_current_task(kernel)];

  old_task.status = Task_Status::READY;

#if defined(SW_TIME_MEASSREMENTS)
  old_task.time_counter += GET_TICK_COUNTER();
  reset_tick_counter();
#endif                                                 // SW_TIME_MEASSREMNTS
  kernel.schedule.active_task_block = new_task_index;  // TODO: refactor

  SET_RETURN_POINT(return_point);

  SAVE_CONTEXT();
  old_task.task_sp_adr = task_sp;

  task_sp =
      kernel.schedule.queue.tasks[index_of_current_task(kernel)].task_sp_adr;
  SET_SP();

  invalidate_scheduled_queue(kernel);
  debug_print("Calling function\n");

  enable_mt();
  enable_interrupts();
  fn();
  /*
   * The second thread of execution must not refer to the parameters passed to
   * the function, since they are probably no longer valid and/or corrupted.
   * */
  asm volatile(
      "cli                 \n\t"
      "call kill_curr_task \n\t" /* Target*/
      "unreachable:        \n\t"
      "jmp unreachable     \n\t"
      : /* No outputs */
      : /* No inputs */
      : "memory");

return_point:
  return new_task_pid;
}

Index create_simplos_task(const ProgmemString &name, const uint8_t priority,
                          Kernel &kernel) {
  uint8_t const index = add_to_task_list(priority, &kernel.schedule.queue);
  Simplos_Task &new_task = kernel.schedule.queue.tasks[index];
  new_task.status = Task_Status::RUNNING;
  new_task.pid = kernel.pid_cnt++;
  new_task.set_name(name);

  debug_print("Created simplos task \"%S\" with pid %u\n", name.progmem_str,
              new_task.pid);
  return index;
}

void kill_current_task(Kernel &kernel) {
  disable_interrupts();
  SCILENT_DISABLE_MT();

  uint8_t const curr_task_index = index_of_current_task(kernel);

  Simplos_Task &task = kernel.schedule.queue.tasks[curr_task_index];
  task.status = Task_Status::EMPTY;
  task.task_sp_adr = task_sp_range_high(curr_task_index);
  task.set_name(progmem_string(""));
  kernel.ended_task_time_counter += task.time_counter;
  invalidate_scheduled_queue(kernel);
  k_yield();  // re-enables interrupts.
}

Simplos_Task *get_task(pid_type pid, Kernel &kernel) {
  SCILENT_DISABLE_MT();
  for (uint8_t t = 0; t < tasks_max; ++t) {
    if (kernel.schedule.queue.tasks[t].pid == pid) {
      SCILENT_ENABLE_MT();
      return &kernel.schedule.queue.tasks[t];
    }
  }
  SCILENT_ENABLE_MT();
  return nullptr;
}

Task_Status task_status(pid_type pid, Kernel &kernel) {
  Simplos_Task *task = get_task(pid, kernel);
  if (task == nullptr) {
    return Task_Status::EMPTY;  // FIXME: bad choise
  }
  return task->status;
}

void init_memory(void) {
  for (uint8_t *canary_adr = (uint8_t *)canary_start;
       (size_t)canary_adr >= canary_end; canary_adr--) {
    write_mm(canary_adr, canary_value);
  }
  ASSERT_EQ(read_from_mm_adr(0x204), canary_value, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x203), canary_value, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x202), canary_value, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x201), canary_value, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x200), canary_value, "0x%X",
            "Failed to write register os stack canary!");
}
