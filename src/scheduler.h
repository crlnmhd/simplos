#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index for the schedule and return it.
*/
void select_next_task(void);

extern Scheduler volatile* volatile simplos_schedule;

// void print_tasks(void);

#endif  // SCHEDULER_H_