#include "os.h"

#include "io_helpers.h"
#include "simplos.h"
#include "simplos_types.h"

void yield(void) { k_yield(); }

uint8_t rank(void) { return simplos_schedule->queue.curr_task_index; }
uint16_t pid(void) {
  return simplos_schedule->queue
      .task_queue[simplos_schedule->queue.curr_task_index]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority) {
  uint16_t const pid = spawn_task(fn, priority);

  cprint("done spawning task ---- new pid is %d\n", pid);
  return pid;
}

void kill_curr_task(void) { kill_current_task(); }

void set_priority(uint8_t const priority) {
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .priority = priority;
}

void wait_for_task_finnish(pid_t pid) {
  while (task_status(pid) != EMPTY) {
    yield();
  }
}

void terminate(void) {
  cli();
  cprint("Simplos terminating...\n");
  print_timing_data();
  for (;;)
    ;
}

void *malloc(size_t bytes) {
  if (bytes == 0) {
    return NULL;
  }
  // dprint("Heap map located at: %p\n", kernel->heap_mapping);
  /*
  uint8_t rem = bytes % HEAP_PAGE_SIZE;
  uint8_t div = bytes / HEAP_PAGE_SIZE;
  uint8_t chunks_needed = 1;
  */
  return NULL;
}
