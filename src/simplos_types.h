#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#define TASKS_MAX 5
#define TASK_MEMORY_BYTES 600
#define ALLOCABLE_MEMORY_BYTES 1024

// TODO place "empty" in this enum.
enum Task_Status {
  SLEEPING,
  READY,
  DONE,
  RUNNING,
};

typedef struct Simplos_Task_t {
  bool empty;
  bool spawning;
  uint16_t task_sp_adr;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
  uint32_t sleep_until;  // ticks
} Simplos_Task_t;
typedef Simplos_Task_t volatile Simplos_Task;

typedef struct Task_Queue_t {
  Simplos_Task_t volatile task_queue[TASKS_MAX];
  uint8_t volatile curr_task_index;
} Task_Queue_t;
typedef Task_Queue_t volatile Task_Queue;

typedef struct Scheduler_t {
  Task_Queue queue;
  // uint8_t processes;
  bool enabled;
} Scheduler_t;
typedef Scheduler_t volatile Scheduler;

#endif  // SIMPLOS_TYPES_H_
