#ifndef SIMPLOS_TYPES_H_
#define SIMPLOS_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "memory_layout.hpp"
#include "progmem.hpp"

enum class Task_Status {
  SLEEPING,
  READY,
  RUNNING,
  EMPTY,
  SCHEDULER,
};

class Simplos_Task {
 public:
  Simplos_Task()
      : time_counter(),
        name(),
        task_sp_adr(0),
        pid(0),
        task_memory_block(0),
        priority(0),
        status(Task_Status::EMPTY){};

  void set_name(const ProgmemString &new_name) { this->name = new_name; }

  uint32_t time_counter;
  ProgmemString name;
  uint16_t task_sp_adr;
  uint16_t pid;
  uint8_t task_memory_block;
  uint8_t priority;
  enum Task_Status status;
};

class Task_Queue {
 public:
  Task_Queue() : tasks(), task_index_queue(), queue_position(0) {}

  Simplos_Task tasks[tasks_max];
  uint8_t task_index_queue[task_queue_length];
  uint8_t queue_position;
};

class Scheduler {
 public:
  Scheduler()
      : queue(), os_task_sp(0), active_task_block(0), task_scheduled(0) {}

  Task_Queue queue;
  uint16_t os_task_sp;
  uint8_t active_task_block;
  uint8_t task_scheduled;  // used as a boolean
};

typedef uint16_t pid_type;

// TODO use this for all global variables.
class Kernel {
 public:
  Kernel()
      : schedule(),
        task_RAM_ranges(),
        cs_time_counter(0),
        ended_task_time_counter(0),
        heap_start(0),
        pid_cnt(0) {}

  Scheduler schedule;
  struct MemorySpan task_RAM_ranges[tasks_max];
  uint32_t cs_time_counter;
  uint32_t ended_task_time_counter;
  uint16_t heap_start;  // dynamically determined by the RAM needed for the init
                        // section (main function).
  uint16_t pid_cnt;
};
typedef Kernel Kernel_type;

#endif  // SIMPLOS_TYPES_H_
