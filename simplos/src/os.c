#include "os.h"

#include <string.h>

#include "hal/hal.h"
#include "io_helpers.h"
#include "memory_layout.h"
#include "simplos.h"
#include "simplos_types.h"
#include "timers.h"

#ifdef MOCK_HAL  // FIXME: avoid knowledge about the mock hal
#include <io_helpers.h>
static Kernel *os_kernel_ptr = NULL;

void set_os_kernel(Kernel *kernel) { os_kernel_ptr = kernel; }

#endif  // MOCK_HAL

void set_os_kernel(Kernel *kernel);
void __attribute__((noinline)) yield(void) {
  k_yield();
  ASSERT_EQ(read_from_mm_adr(0x505), 0x0011, "0x%X",
            "unexpected upper end of return address");
  ASSERT_EQ(read_from_mm_adr(0x507), 0x00b6, "0x%X",
            "unexpected lower end of return address");
}

Kernel *get_os_kernel(void) {
#ifdef MOCK_HAL
  if (os_kernel_ptr == NULL) {
    FATAL_ERROR("No os kernel set for mock hal\n");
  }
  return os_kernel_ptr;
#else
  return global_kernel;
#endif
}

uint8_t rank(void) { return INDEX_OF_CURRENT_TASK(get_os_kernel()); }
uint16_t pid(void) {
  return get_os_kernel()
      ->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(get_os_kernel())]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority, char const *name) {
  // Validate name length
  uint16_t const name_length = strlen(name);
  ASSERT(name_length <= FUNCTION_NAME_MAX_LENGTH,
         "Function name length exceeded.");

  uint16_t const spawned_task_pid =
      spawn_task(fn, priority, name, get_os_kernel());

  cprint("done spawning task \"%s\"---- new pid is %u\n", name,
         spawned_task_pid);

  return spawned_task_pid;
}

void kill_curr_task(void) { kill_current_task(get_os_kernel()); }

void set_priority(uint8_t const priority) {
  get_os_kernel()
      ->schedule.queue.tasks[INDEX_OF_CURRENT_TASK(get_os_kernel())]
      .priority = priority;
}

void wait_for_task_finnish(pid_t pid) {
  while (task_status(pid, get_os_kernel()) != EMPTY) {
    yield();
  }
}

NORETURN void terminate(void) {
  disable_interrupts();
  cprint("Simplos terminating...\n");
  print_timing_data(get_os_kernel());
  halt();
  __builtin_unreachable();
}

NORETURN void halt(void) {
  print("HALTING\n");
  halt_exec();
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
