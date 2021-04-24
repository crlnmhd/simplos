#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index and return a pointer to that task.
*/
uint8_t select_next_task(Scheduler* scheduler);

void print_tasks(Scheduler* scheduler);

#endif  // SCHEDULER_H_