#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "memory_layout.h"

#define TASKS_MAX 5U

enum Task_Status {
  SLEEPING,
  READY,
  RUNNING,
  EMPTY,
  SCHEDULER,
};

typedef struct Simplos_Task_type {
  uint32_t time_counter;
  uint16_t task_sp_adr;
  uint16_t pid;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
} Simplos_Task_type;
typedef Simplos_Task_type volatile Simplos_Task;
typedef Simplos_Task volatile *volatile taskptr_type;

typedef struct Task_Queue_type {
  Simplos_Task_type tasks[TASKS_MAX];
  uint8_t task_index_queue[TASK_QUEUE_LENGTH];
  uint8_t queue_position;
} Task_Queue_type;
typedef Task_Queue_type volatile Task_Queue;

typedef struct Scheduler_type {
  Task_Queue queue;
  uint16_t os_task_sp;
  uint8_t active_task_block;
  uint8_t task_scheduled;  // used as a boolean
} Scheduler_type;
typedef Scheduler_type Scheduler;

typedef uint16_t pid_type;

// TODO use this for all global variables.
typedef struct Kernel_type {
  Scheduler schedule;
  char task_names[TASKS_MAX][FUNCTION_NAME_MAX_LENGTH + 1];
  struct MemorySpan task_RAM_ranges[TASKS_MAX];
  uint32_t cs_time_counter;
  uint32_t ended_task_time_counter;
  uint16_t heap_start;  // dynamically determined by the RAM needed for the init
                        // section (main function).
  uint16_t pid_cnt;
} Kernel_type;
typedef Kernel_type Kernel;

#endif  // SIMPLOS_TYPES_H_
