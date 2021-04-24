#include "io_helpers.h"

#include <stdio.h>
#include <string.h>

#include "interupts.h"
#include "memory.h"

void print_task(Simplos_Task* task, bool use_debug) {
  if (task == NULL) {
    fatal_error("Error, task is NULL\n");
  }
  char status[10];
  status[9] = '\0';
  switch (task->status) {
    case READY:
      strcpy(status, "READY");
      break;
    case DONE:
      strcpy(status, "DONE");
      break;
    case RUNNING:
      strcpy(status, "RUNNING");
      break;
    default:
      strcpy(status, "OTHER");
      break;
  }

  int const buf_max_size = 100;
  char buf[buf_max_size];

  char const* empty = task->empty ? "EMPTY" : "";
  char const* spawning = task->spawning ? "SPAWNING" : "";

  uint16_t upper_bound = task_default_sp(task->task_memory_block);
  uint16_t lower_bound = task->task_memory_block == 0
                             ? stack_end()
                             : task_default_sp(task->task_memory_block - 1);
  task_default_sp(task->task_memory_block - 1);

  char const* out_of_bounds =
      (lower_bound <= task->task_sp_adr && task->task_sp_adr <= upper_bound)
          ? ""
          : "OUT OF BOUND";

  int chars = snprintf(buf, buf_max_size,
                       "Task at block %d: SP=0x%X,\t: %s,\t%s: "
                       "\tPRIORITY: %d, \t%s, \t%s#\n",
                       task->task_memory_block, task->task_sp_adr, status,
                       empty, task->priority, spawning, out_of_bounds);
  if (chars < 0 || chars > buf_max_size) {
    fatal_error("Error, print_task failed to write to buffer");
  }
  if (use_debug) {
    dprint(buf);
  } else {
    printf(buf);
  }
}

void fatal_error(char const* msg) {
  cli();
  printf("FATAL ERROR: %s\n", msg);
  for (;;)
    ;
}