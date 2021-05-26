#include "os.h"

#include "simplos.h"

void yield(void) { k_yield(); }

// FIXME give each task a unique id.
uint8_t rank(void) { return simplos_schedule->queue.curr_task_index; }
uint16_t pid(void) {
  return simplos_schedule->queue
      .task_queue[simplos_schedule->queue.curr_task_index]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority) {
  spawn_task(fn, priority);
  // dprint(PSTR("done spawning task ----\n"));
  // dprint(PSTR("1 done spawning task ----\n"));
  // dprint(PSTR("2 done spawning task ----\n"));
  // dprint(PSTR("3 done spawning task ----\n"));
  // dprint(PSTR("4 done spawning task ----\n"));
  uint16_t const pid = simplos_schedule->queue
                           .task_queue[simplos_schedule->queue.curr_task_index]
                           .pid;
  cprint("done spawning task ----\n");
  return pid;
}

void kill_curr_task(void) { kill_current_task(); }

void set_priority(uint8_t const priority) {
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .priority = priority;
}

void terminate(void) {
  cli();
  cprint("Simplos terminating...\n");
  for (;;)
    ;
}