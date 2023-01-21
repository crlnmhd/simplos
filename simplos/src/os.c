#include "os.h"

#include <string.h>

#include "io_helpers.h"
#include "memory_layout.h"
#include "simplos.h"
#include "simplos_types.h"

// void yield(void) {
void __attribute__((noinline)) yield(void) {
  //  cprint("SP is 0x%X\n", SP);
  //  asm volatile("nop");
  //  cprint("Hi from yield!\n");
  k_yield();
  ASSERT_EQ((*(uint16_t *)0x505), 0x0011, "0x%X",
            "unexpected upper end of return address");
  ASSERT_EQ((*(uint8_t *)0x507), 0x00b6, "0x%X",
            "unexpected lower end of return address");
  //  cprint("SP post pop 0x%X\n", SP);
}

uint8_t rank(void) { return kernel->schedule.queue.curr_task_index; }
uint16_t pid(void) {
  return kernel->schedule.queue.tasks[kernel->schedule.queue.curr_task_index]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority, char const *name) {
  // Validate name length
  uint16_t const name_length = strlen(name);
  ASSERT(name_length <= FUNCTION_NAME_MAX_LENGTH,
         "Function name length exceeded.");

  uint16_t const pid = spawn_task(fn, priority, name);

  cprint("done spawning task \"%s\"---- new pid is %d\n", name, pid);
  return pid;
}

void kill_curr_task(void) { kill_current_task(); }

void set_priority(uint8_t const priority) {
  kernel->schedule.queue.tasks[kernel->schedule.queue.curr_task_index]
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
