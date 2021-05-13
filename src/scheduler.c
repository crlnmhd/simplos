#include "scheduler.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>

#include "io_helpers.h"
#include "serial.h"
#include "simplos.h"

#define USE_STATIC

// TODO better scheduling. This is just a placeholder.

uint8_t select_next_task() {
  // This defaults to the idle task.
  // print_schedule(simplos_schedule);

  // cprint("At block: %d\n", simplos_schedule->queue.curr_task_index);
  for (uint8_t i = 1; i <= TASKS_MAX; ++i) {
    uint8_t const next_candidate_index =
        (simplos_schedule->queue.curr_task_index + i) % TASKS_MAX;

    if (simplos_schedule->queue.task_queue[next_candidate_index].status ==
        READY) {
      simplos_schedule->queue.curr_task_index = next_candidate_index;
      // cprint("selected task  %d (SP=0x%X) using circular scheduling.\n",
      //        simplos_schedule->queue.curr_task_index,
      //        simplos_schedule->queue
      //            .task_queue[simplos_schedule->queue.curr_task_index]
      //            .task_sp_adr);
      break;
    }
  }
  if (simplos_schedule->queue
          .task_queue[simplos_schedule->queue.curr_task_index]
          .status == EMPTY) {
    fatal_error("No task ready to run!\n");
  }
  return simplos_schedule->queue.curr_task_index;
}
