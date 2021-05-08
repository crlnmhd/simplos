#include "io_helpers.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "interupts.h"
#include "memory.h"

void print_task(Simplos_Task* task, bool const checks) {
  if (task == NULL) {
    fatal_error("Error, task is NULL\n");
  }
  cprint("Task at block %d:", task->task_memory_block);
  cprint("  Priority %d", task->priority);
  cprint("  SP = 0x%X", task->task_sp_adr);
  if (task->empty) {
    cprint("  EMPTY");
  }
  if (task->spawning) {
    cprint("  SPAWNING");
  }
  switch (task->status) {
    case READY:
      cprint("  READY");
      break;
    case DONE:
      cprint("  DONE");
      break;
    case RUNNING:
      cprint("  RUNNING");
      break;
    default:
      cprint("  OTHER");
  }
  cprint("\n");

  // dprint("Debug -- task memory block: %d\n", task->task_memory_block);

  if (checks) {
    uint16_t upper_bound = task_default_sp(task->task_memory_block);
    uint16_t lower_bound = task->task_memory_block == 0
                               ? stack_end()
                               : task_default_sp(task->task_memory_block - 1);

    bool const sp_within_bounds =
        (lower_bound <= task->task_sp_adr && task->task_sp_adr <= upper_bound);

    if (!sp_within_bounds) {
      fatal_error("Task %d SP = 0x%X is of bounds!", task->task_memory_block,
                  task->task_sp_adr);
    }
  }
}

void print_schedule(Scheduler* scheduler) {
  cprint(" -- Schedule --\n");
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = &scheduler->queue.task_queue[i];
    // dprint("DEBUG:: has mem block: %d\n", task->task_memory_block);
    print_task(task, true);
  }
}