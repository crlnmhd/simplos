#include "simplos.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hal/context_switch_macros.h"
#include "hal/hal.h"
#include "io_helpers.h"
#include "memory.h"
#include "memory_layout.h"
#include "os.h"
#include "scheduler.h"
#include "simplos_types.h"
#include "timers.h"

void init_task_list(Task_Queue *queue) {
  warn_if_task_memory_can_not_be_divided_evenly_between_tasks();
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task *task = &queue->tasks[i];
    task->task_memory_block = i;
    task->time_counter = 0;
    task->task_sp_adr = task_sp_range_high(task->task_memory_block);
    debug_print("Initiating mem block %u at 0x%X-0x%X\n", i,
                task_sp_range_low(task->task_memory_block), task->task_sp_adr);
    task->status = EMPTY;
  }
}

uint8_t add_to_task_list(uint8_t priority, Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task *task = (Simplos_Task *)&queue->tasks[i];
    // Take if available
    if (task->status == EMPTY) {
      verbose_print("Initiating space for new function at block %u\n", i);
      task->priority = priority;
      task->status = READY;
      task->task_sp_adr = task_sp_range_high(task->task_memory_block);
      task->pid = UINT16_MAX;
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

void init_schedule(Kernel *kernel) {
  kernel->schedule.os_task_sp = OS_STACK_START;
  init_task_list(&(kernel->schedule.queue));
  kernel->cs_time_counter = 0;
  kernel->schedule.active_task_block = 0;
}

__attribute__((noinline, naked)) void k_yield(void) {
  asm volatile("cli" ::: "memory");
  CONTEXT_SWTICH();
  asm volatile(
      "sei  \n\t"
      "ret  \n\t"
      : /* No outputs*/
      : /* No input*/
      : "memory");
}

void init_kernel(Kernel *kernel) {
  kernel->schedule.queue.queue_position = 0;
  for (uint8_t i = 0; i < TASKS_MAX; i++) {
    // Empty task name.
    set_task_name(i, "", kernel);

    // Set task RAM range.
    struct MemorySpan *task_stack_range = &kernel->task_RAM_ranges[i];
    task_stack_range->high = task_sp_range_high(i);
    task_stack_range->low = task_sp_range_low(i);
  }
}

void verify_that_kernel_is_uninitilized(Kernel *kernel) {
  if (kernel->pid_cnt != 0) {
    FATAL_ERROR("Attempted to re-run startup process. Memory wrap-arround?");
  }
}

pid_type spawn_task(void (*fn)(void), uint8_t const priority, char const *name,
                    Kernel *kernel) {
  disable_interrupts();
  SCILENT_DISABLE_MT();
  uint8_t const new_task_index = create_simplos_task(name, priority, kernel);

  Simplos_Task *new_task = &kernel->schedule.queue.tasks[new_task_index];
  new_task->status = RUNNING;

  uint16_t const new_task_pid = new_task->pid;

  Simplos_Task *old_task =
      &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)];

  old_task->status = READY;

#if defined(SW_TIME_MEASSREMENTS)
  old_task->time_counter += GET_TICK_COUNTER();
  reset_tick_counter();
#endif  // SW_TIME_MEASSREMNTS
  INDEX_OF_CURRENT_TASK(kernel) = new_task_index;

  SET_RETURN_POINT(return_point);

  SAVE_CONTEXT();
  old_task->task_sp_adr = task_sp;

  task_sp =
      kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(kernel)].task_sp_adr;
  SET_SP();

  invalidate_scheduled_queue(kernel);
  debug_print("Calling function\n");

  ENABLE_MT();
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

inline void set_task_name(const Index task_index, const char *name,
                          Kernel *kernel) {
  strlcpy((char *)kernel->task_names[task_index], name,
          FUNCTION_NAME_MAX_LENGTH + 1);
}

inline Index create_simplos_task(const char *name, const uint8_t priority,
                                 Kernel *kernel) {
  uint8_t const index = add_to_task_list(priority, &kernel->schedule.queue);
  Simplos_Task *new_task = &kernel->schedule.queue.tasks[index];
  new_task->status = RUNNING;
  new_task->pid = kernel->pid_cnt++;
  set_task_name(index, name, kernel);

  debug_print("Created simplos task %s with pid %u\n", name, new_task->pid);
  return index;
}

uint16_t num_context_switch_overhead_bytes(void) {
  const uint16_t num_registers = 32;
  const uint16_t num_pc_bytes = 3;
  const uint16_t num_sreg_bytes = 1;
  return num_pc_bytes + num_registers + num_sreg_bytes;
}

void kill_current_task(Kernel *kernel) {
  disable_interrupts();
  SCILENT_DISABLE_MT();

  uint8_t const curr_task_index = INDEX_OF_CURRENT_TASK(kernel);

  Simplos_Task *task = &kernel->schedule.queue.tasks[curr_task_index];
  task->status = EMPTY;
  task->task_sp_adr = task_sp_range_high(curr_task_index);
  kernel->task_names[curr_task_index][0] = '\0';
  kernel->ended_task_time_counter += task->time_counter;
  invalidate_scheduled_queue(kernel);
  k_yield();  // re-enables interrupts.
}

Simplos_Task *get_task(pid_type pid, Kernel *kernel) {
  SCILENT_DISABLE_MT();
  for (uint8_t t = 0; t < TASKS_MAX; ++t) {
    if (kernel->schedule.queue.tasks[t].pid == pid) {
      SCILENT_ENABLE_MT();
      return &kernel->schedule.queue.tasks[t];
    }
  }
  SCILENT_ENABLE_MT();
  return NULL;
}

enum Task_Status task_status(pid_type pid, Kernel *kernel) {
  Simplos_Task *task = get_task(pid, kernel);
  if (task == NULL) {
    return EMPTY;
  }
  return task->status;
}

void init_memory(void) {
  for (uint8_t *canary_adr = (uint8_t *)CANARY_START;
       (size_t)canary_adr >= CANARY_END; canary_adr--) {
    write_mm(canary_adr, CANARY_VALUE);
  }
  ASSERT_EQ(read_from_mm_adr(0x204), CANARY_VALUE, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x203), CANARY_VALUE, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x202), CANARY_VALUE, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x201), CANARY_VALUE, "0x%X",
            "Failed to write canary between registers and stack!");
  ASSERT_EQ(read_from_mm_adr(0x200), CANARY_VALUE, "0x%X",
            "Failed to write register os stack canary!");
}
