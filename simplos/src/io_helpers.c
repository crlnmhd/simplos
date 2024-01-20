
#include "io_helpers.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "os.h"
#include "simplos.h"

NO_MT void print_task(taskptr_type task) {
  if (task == NULL) {
    FATAL_ERROR("Error, task is NULL\n");
  }

  uint8_t const task_index = task->task_memory_block;
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
  char const *task_name = global_kernel->task_names[task_index];
  END_DISCARD_VOLATILE_QUALIFIER_WARNING()
  cprint("Task: \"%s\". Block: %u", task_name, task_index);
  cprint(" PID: %u", task->pid);
  cprint(" Priority: %u", task->priority);
  cprint(" SP: 0x%X", task->task_sp_adr);
  struct StackRange task_ram_range =
      *(struct StackRange *)&global_kernel->task_RAM_ranges[task_index];
  cprint(" [0x%X - 0x%X]\n", task_ram_range.low, task_ram_range.high);

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
}

void print_schedule(void) {
  print(" -- Schedule --\n");
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task *task = &global_kernel->schedule.queue.tasks[i];
    // dprint("DEBUG:: has mem block: %d\n", task->task_memory_block);
    print_task(task);
  }
  cprint(" --  end of schedule --\n")
}

NO_MT void print_timing_data(void) {
#if defined(SW_TIME_MEASSREMENTS)
  cprint("Timing data:\nOS: %u\n", global_kernel->cs_time_counter);
  // FIXME risk of overflow!
  uint32_t running_total = 0;
  for (uint8_t i = 0; i < TASKS_MAX; i++) {
    Simplos_Task *task = &global_kernel->schedule->queue.tasks[i];
    if (task->status != EMPTY) {
      cprint("Task %u: %u\n", i, task->time_counter);
      running_total += task->time_counter;
    }
  }
  float cs_time_fraction =
      (float)global_kernel->cs_time_counter /
      (float)(running_total + global_kernel->ended_task_time_counter);
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
