#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#define TASKS_MAX 5
#define TASK_MEMORY_BYTES 512
#define ALLOCABLE_MEMORY_BYTES 1024

// TODO place "empty" in this enum.
enum Task_Status {
  SLEEPING,
  READY,
  DONE,
  RUNNING,
};

typedef struct {
  bool empty;
  uint16_t task_sp;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
  uint32_t sleep_until;  // ticks
} Simplos_Task;

typedef struct {
  Simplos_Task task_queue[TASKS_MAX];
  uint8_t curr_task_index;
} Task_Queue;

typedef struct {
  bool force_prev;
  uint8_t prev_task;
  Task_Queue queue;
  // uint8_t processes;
  bool enabled;
} Scheduler;

#endif  // SIMPLOS_TYPES_H_
