
#include "io_helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "scheduler.h"
#include "simplos.h"
#include "timers.h"

NO_MT void print_task(taskptr_t task, bool const checks) {
  if (task == NULL) {
    fatal_error("Error, task is NULL\n");
  }

  uint8_t const task_index = task->task_memory_block;
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
  char const *task_name = kernel->task_names[task_index];
  END_DISCARD_VOLATILE_QUALIFIER_WARNING()
  cprint("Task \"%s\" at block %d:", task_name, task_index);
  cprint(" PID: %d", task->pid);
  cprint(" Priority %d", task->priority);
  cprint(" SP = 0x%X", task->task_sp_adr);

  switch (task->status) {
    case READY:
      cprint("  READY");
      break;
    case EMPTY:
      cprint("  EMPTY");
      break;
    case RUNNING:
      cprint("  RUNNING");
      break;
    case SLEEPING:
      cprint("  SLEEPING");
      break;
    default:
      cprint("  STATUS UNKNOWN");
  }
  cprint("\n");

  // dprint("Debug -- task memory block: %d\n", task->task_memory_block);

  if (checks) {
    assert_stack_integrity(task);
  }
}

NO_MT void print_schedule(void) {
  cprint(" -- Schedule --\n");
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task *task = &simplos_schedule->queue.task_queue[i];
    // dprint("DEBUG:: has mem block: %d\n", task->task_memory_block);
    print_task(task, false);
  }
}

NO_MT void print_timing_data(void) {
#if defined(SW_TIME_MEASSREMENTS)
  cprint("Timing data:\nOS: %d\n", kernel->cs_time_counter);
  // FIXME risk of overflow!
  uint32_t running_total = 0;
  for (uint8_t i = 0; i < TASKS_MAX; i++) {
    Simplos_Task *task = &simplos_schedule->queue.task_queue[i];
    if (task->status != EMPTY) {
      cprint("Task %d: %d\n", i, task->time_counter);
      running_total += task->time_counter;
    }
  }
  float cs_time_fraction =
      (float)kernel->cs_time_counter /
      (float)(running_total + kernel->ended_task_time_counter);
  cprint("Total fraction: %f\n", cs_time_fraction);
#else
  cprint("Printing timing data: DISABLED\n");
#endif
}
// void print_task_stack(Simplos_Task * task){
//   size_t const stack_start = task_default_sp(task->task_memory_block);
//   DISABLE_MT();
//   cprint("Stack of task %d (in hexadecimal):",task->task_memory_block)

//   for(size_t s = stack_start; s >= stack_start - TASK_MEMORY_BYTES; -s){
//     cprint("0x%X ")
//   }

// }
//
