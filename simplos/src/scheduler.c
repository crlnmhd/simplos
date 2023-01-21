#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"

// TODO better scheduling. This is just a placeholder.

NO_MT void static_cyclic_scheduler(void) {
  // This defaults to the idle task.
  // print_schedule(kernel->schedule);

  // cprint("At block: %d\n", kernel->schedule.queue.curr_task_index);
  for (uint8_t i = 1; i <= TASKS_MAX; ++i) {
    uint8_t const next_candidate_index =
        ((uint8_t)(INDEX_OF_CURRENT_TASK + i) % TASKS_MAX);

    if (kernel->schedule.queue.tasks[next_candidate_index].status == READY) {
      INDEX_OF_CURRENT_TASK = next_candidate_index;
      cprint(
          "selected task  %d \"%s\" with SP=0x%X using circular scheduling.\n",
          INDEX_OF_CURRENT_TASK, kernel->task_names[INDEX_OF_CURRENT_TASK],
          kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].task_sp_adr);
      break;
    }
  }
  if (kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].status == EMPTY) {
    fatal_error("No task ready to run!\n");
  }
}

void prioity_scheduler(void) {
  ASSERT(false, "Priority schduling is not implemented!\n");
}

void select_next_task(void) {
#if defined(PRIORITY_SCHEDULING)
  prioity_scheduler();
#else
  static_cyclic_scheduler();
#endif
}
