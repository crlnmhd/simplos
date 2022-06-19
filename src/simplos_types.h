#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdint.h>

#include "memory_layout.h"

#define TASKS_MAX 5

#if defined(HW_TIME_MEASSUREMENTS)
#define OS_TASK_BLOCK 0x0F
#define EXEC_FINNISHED 0x0E
#endif

enum Task_Status { SLEEPING, READY, RUNNING, EMPTY };

typedef struct Simplos_Task_t {
  uint32_t time_counter;
  uint16_t task_sp_adr;
  uint16_t pid;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
} Simplos_Task_t;
typedef Simplos_Task_t volatile Simplos_Task;
typedef Simplos_Task volatile *volatile taskptr_t;

typedef struct Task_Queue_t {
  Simplos_Task_t volatile task_queue[TASKS_MAX];
  uint8_t volatile curr_task_index;
} Task_Queue_t;
typedef Task_Queue_t volatile Task_Queue;

typedef struct Scheduler_t {
  Task_Queue queue;
  volatile uint16_t os_task_sp;
  bool enabled;
} Scheduler_t;
typedef Scheduler_t volatile Scheduler;

typedef uint16_t pid_t;

// TODO use this for all global variables.
typedef struct Kernel_t {
  char task_names[TASKS_MAX][FUNCTION_NAME_MAX_LENGTH + 1];
  uint8_t heap_mapping[HEAP_CHUNKS];
  uint32_t cs_time_counter;
  uint32_t ended_task_time_counter;
} Kernel_t;
typedef Kernel_t volatile Kernel;

#endif  // SIMPLOS_TYPES_H_
