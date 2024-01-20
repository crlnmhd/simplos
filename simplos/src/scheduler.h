#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index for the schedule and return it.
*/
void select_next_task(Kernel *);

void start_scheduler(Kernel *);
/*
 * Wrapper around start_scheduler that injects the (global) kernel.
 *
 * TODO: impplement parameters to spawn() that would allow for the kernel to be
 * passed as an argument, removing the need for this wrapper.
 * */
void start_scheduler_with_os_kernel(void);
// void print_tasks(void);

#endif  // SCHEDULER_H_
