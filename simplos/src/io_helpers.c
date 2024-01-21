
#include "io_helpers.h"

#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "os.h"
#include "simplos.h"

void print_task(Simplos_Task *task, Kernel *kernel) {
  if (task == NULL) {
    FATAL_ERROR("Error, task is NULL\n");
  }

  uint8_t const task_index = task->task_memory_block;
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
  char const *task_name = kernel->task_names[task_index];
  END_DISCARD_VOLATILE_QUALIFIER_WARNING()
  if (kernel->schedule.active_task_block == task->task_memory_block) {
    debug_print("* ");
  } else {
    debug_print("  ");
  }
  debug_print("Task: \"%s\". Block: %u", task_name, task_index);
  debug_print(" PID: %u", task->pid);
  debug_print(" Priority: %u", task->priority);
  debug_print(" SP: 0x%X", task->task_sp_adr);
  struct MemorySpan task_ram_range =
      *(struct MemorySpan *)&kernel->task_RAM_ranges[task_index];
  debug_print(" [0x%X - 0x%X] ", task_ram_range.low, task_ram_range.high);

  switch (task->status) {
    case READY:
      debug_print("  READY");
      break;
    case EMPTY:
      debug_print("  EMPTY");
      break;
    case RUNNING:
      debug_print("  RUNNING");
      break;
    case SLEEPING:
      debug_print("  SLEEPING");
      break;
    case SCHEDULER:
      debug_print("  SCHEDULER");
      break;
    default:
      debug_print("  STATUS UNKNOWN");
  }
  debug_print("\n");

  // debug_print("Debug -- task memory block: %d\n", task->task_memory_block);
}

void print_schedule(Kernel *kernel) {
  debug_print(" -- Schedule --\n");
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task *task = &kernel->schedule.queue.tasks[i];
    // debug_print("DEBUG:: has mem block: %d\n", task->task_memory_block);
    print_task(task, kernel);
  }
  debug_print(" --  end of schedule --\n");
}

void print_timing_data(__attribute__((unused)) Kernel *kernel) {
#if defined(SW_TIME_MEASSREMENTS)
  debug_print("Timing data:\nOS: %u\n", kernel->cs_time_counter);
  // FIXME risk of overflow!
  uint32_t running_total = 0;
  for (uint8_t i = 0; i < TASKS_MAX; i++) {
    Simplos_Task *task = &global_kernel->schedule->queue.tasks[i];
    if (task->status != EMPTY) {
      debug_print("Task %u: %u\n", i, task->time_counter);
      running_total += task->time_counter;
    }
  }
  float cs_time_fraction =
      (float)kernel->cs_time_counter /
      (float)(running_total + kernel->ended_task_time_counter);
  debug_print("Total fraction: %f\n", cs_time_fraction);
#else
  debug_print("Printing timing data: DISABLED\n");
#endif
}

void do_nothing_ignoreing_passed_parameters(__attribute__((unused))
                                            const char *fmt,
                                            ...) {
  // Nothing
}

// void print_task_stack(Simplos_Task * task){
//   size_t const stack_start = task_default_sp(task->task_memory_block);
//   DISABLE_MT();
//   debug_print("Stack of task %d (in hexadecimal):",task->task_memory_block)

//   for(size_t s = stack_start; s >= stack_start - TASK_MEMORY_BYTES; -s){
//     debug_print("0x%X ")
//   }

// }
//
