#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index and return a pointer to that task.
*/
volatile Simplos_Task* select_next_task(volatile Scheduler* scheduler);

void print_tasks(volatile Scheduler* scheduler);

#endif // SCHEDULER_H_