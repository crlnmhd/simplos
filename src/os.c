#include "os.h"

#include "simplos.h"

void yield(void) { k_yield(); }

uint8_t rank(void) { return simplos_schedule->queue.curr_task_index; }
uint16_t pid(void) {
  return simplos_schedule->queue
      .task_queue[simplos_schedule->queue.curr_task_index]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority) {
  uint16_t const pid = spawn_task(fn, priority);

  // dprint(PSTR("done spawning task ----\n"));
  // dprint(PSTR("1 done spawning task ----\n"));
  // dprint(PSTR("2 done spawning task ----\n"));
  // dprint(PSTR("3 done spawning task ----\n"));
  // dprint(PSTR("4 done spawning task ----\n"));

  cprint("done spawning task ----. new pid is %d\n", pid);
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
  for (;;)
    ;
}