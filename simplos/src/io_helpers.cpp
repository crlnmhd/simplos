
#include "io_helpers.hpp"

#include <stdio.h>  // FIXME delete?
#include <string.h>

#include "memory.hpp"

void print_task(Simplos_Task &task, Kernel &kernel) {
  uint8_t const task_index = task.task_memory_block;
  if (kernel.schedule.active_task_block == task.task_memory_block) {
    debug_print("* ");
  } else {
    debug_print("  ");
  }
  debug_print("Task: ");
  if (task.name.progmem_str == nullptr) {
    debug_print("\"\"");
  } else {
    debug_print("\"%S\"", task.name.progmem_str);
  }
  debug_print(" Block: %u", task_index);
  debug_print(" PID: %u", task.pid);
  debug_print(" Priority: %u", task.priority);
  debug_print(" SP: 0x%X", task.task_sp_adr);
  MemorySpan task_ram_range = *&kernel.task_RAM_ranges[task_index];
  debug_print(" [0x%X - 0x%X] ", task_ram_range.low, task_ram_range.high);

  switch (task.status) {
    case Task_Status::READY:
      debug_print("  READY");
      break;
    case Task_Status::EMPTY:
      debug_print("  EMPTY");
      break;
    case Task_Status::RUNNING:
      debug_print("  RUNNING");
      break;
    case Task_Status::SLEEPING:
      debug_print("  SLEEPING");
      break;
    case Task_Status::SCHEDULER:
      debug_print("  SCHEDULER");
      break;
    default:
      debug_print("  STATUS UNKNOWN");
  }
  debug_print("\n");

  // debug_print("Debug -- task memory block: %d\n", task.task_memory_block);
}

void print_schedule(Kernel &kernel) {
  debug_print(" -- Schedule --\n");
  for (uint8_t i = 0; i < tasks_max; ++i) {
    Simplos_Task &task = kernel.schedule.queue.tasks[i];
    // debug_print("DEBUG:: has mem block: %d\n", task.task_memory_block);
    print_task(task, kernel);
  }
  debug_print(" --  end of schedule --\n");
}

void print_timing_data(__attribute__((unused)) Kernel *kernel) {
#if defined(SW_TIME_MEASSREMENTS)
  debug_print("Timing data:\nOS: %u\n", kernel->cs_time_counter);
  // FIXME risk of overflow!
  uint32_t running_total = 0;
  for (uint8_t i = 0; i < tasks_max; i++) {
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
//   disable_mt();
//   debug_print("Stack of task %d (in hexadecimal):",task->task_memory_block)

//   for(size_t s = stack_start; s >= stack_start - TASK_MEMORY_BYTES; -s){
//     debug_print("0x%X ")
//   }

// }
//
