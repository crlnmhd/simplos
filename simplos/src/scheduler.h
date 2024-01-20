#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"
/*
  Set curr_task_index for the schedule and return it.
*/
void select_next_task(Kernel *);

void start_scheduler(Kernel *);

/*
 * fills list tasks_block_list of length num_tasks unique active tasks.
 * Return value: the number of unique tasks.
 *
 * Note: any values of tasks_block_list[<return_value -1>] have no meening.
 * */
uint8_t get_active_tasks(uint8_t *tasks_block_list, const uint8_t num_tasks,
                         Kernel *kernel);
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
