#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "simplos_types.h"

void select_next_task(volatile Scheduler* scheduler);

void print_tasks(volatile Scheduler* scheduler);

#endif // SCHEDULER_H_