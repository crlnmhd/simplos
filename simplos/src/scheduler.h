#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index for the schedule and return it.
*/
void select_next_task(Kernel *);

void start_scheduler(Kernel *);

/*
 * reschedules. Public to make it testable.
 * */
void reschedule(Kernel *kernel);
/*
 * Sets the priority order of the tasks queue in out_priority_list.
 * In decreasing order of priority.
 * */
void prioritize_tasks(taskptr_type tasks, volatile uint8_t *out_priority_list);

/*
 * Creates a mask where the bit N is set iff the task with index N is active.
 * */
uint8_t get_active_tasks_mask(Kernel *kernel);
/*
 * Wrapper around start_scheduler that injects the (global) kernel.
 *
 * TODO: impplement parameters to spawn() that would allow for the kernel to be
 * passed as an argument, removing the need for this wrapper.
 * */
#ifndef MOCK_HAL
void start_scheduler_with_os_kernel(void);
#endif  // MOCK_HAL

// void print_tasks(void);

#endif  // SCHEDULER_H_
