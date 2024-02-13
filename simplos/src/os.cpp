#include "os.hpp"

#include <string.h>

#include "hal/hal.hpp"
#include "io_helpers.hpp"
#include "simplos.hpp"
#include "simplos_types.hpp"
#include "timers.hpp"

#ifdef MOCK_HAL  // FIXME: avoid knowledge about the mock hal
#include <io_helpers.hpp>
static Kernel *os_kernel_ptr = nullptr;

void set_os_kernel(Kernel *kernel) { os_kernel_ptr = kernel; }

#endif  // MOCK_HAL

void set_os_kernel(Kernel *kernel);
void __attribute__((noinline)) yield(void) { k_yield(); }

Kernel &get_os_kernel(void) {
#ifdef MOCK_HAL
  if (os_kernel_ptr == nullptr) {
    FATAL_ERROR("No os kernel set for mock hal\n");
  }
  return *os_kernel_ptr;
#else
  return *(const_cast<Kernel *>(global_kernel));
#endif
}

uint8_t rank(void) { return index_of_current_task(get_os_kernel()); }
uint16_t pid(void) {
  return get_os_kernel()
      .schedule.queue.tasks[index_of_current_task(get_os_kernel())]
      .pid;
}

uint16_t spawn(void (*fn)(void), uint8_t const priority,
               const ProgmemString &name) {
  // Validate name length
  debug_print("Calling function located at: %p\n", fn);

  uint16_t const spawned_task_pid =
      spawn_task(fn, priority, name, get_os_kernel());

  debug_print("done spawning task \"%S\"---- new pid is %u\n", name.progmem_str,
              spawned_task_pid);

  return spawned_task_pid;
}

void kill_curr_task(void) { kill_current_task(get_os_kernel()); }

void set_priority(uint8_t const priority) {
  get_os_kernel()
      .schedule.queue.tasks[index_of_current_task(get_os_kernel())]
      .priority = priority;
}

void wait_for_task_finnish(pid_t pid) {
  while (task_status(pid, get_os_kernel()) != Task_Status::EMPTY) {
    yield();
  }
}

NORETURN void terminate(void) {
  disable_interrupts();
  debug_print("Simplos terminating...\n");
  print_timing_data(&get_os_kernel());
  halt();
  __builtin_unreachable();
}

NORETURN void halt(void) {
  print("HALTING\n");
  halt_exec();
}
void *malloc(size_t bytes) {
  if (bytes == 0) {
    return nullptr;
  }
  // debug_print("Heap map located at: %p\n", kernel->heap_mapping);
  /*
  uint8_t rem = bytes % HEAP_PAGE_SIZE;
  uint8_t div = bytes / HEAP_PAGE_SIZE;
  uint8_t chunks_needed = 1;
  */
  return nullptr;
}
