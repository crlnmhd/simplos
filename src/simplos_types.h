#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define TASKS_MAX 5
#define TASK_MEMORY_BYTES 600
#define ALLOCABLE_MEMORY_BYTES 1024

// OS task memory for use within constext switches and alike. Aprox 50 bytes).
// RAMSTART = 0x200
#define OS_RAM_START (0x200 + 0x149)
#define TASK_RAM_END (0x200 + 0x150)

// TODO place "empty" in this enum.
enum Task_Status { SLEEPING, READY, RUNNING, EMPTY };

typedef struct Simplos_Task_t {
  uint16_t task_sp_adr;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
  uint32_t sleep_until;  // ticks
  uint16_t pid;
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
  uint16_t volatile* volatile os_task_sp;

} Scheduler_t;
typedef Scheduler_t volatile Scheduler;

#endif  // SIMPLOS_TYPES_H_
