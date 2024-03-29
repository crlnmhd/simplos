#if !defined(SIMPLOS_H_)
#define SIMPLOS_H_

#include "inline.hpp"
#include "memory.hpp"
#include "simplos_types.hpp"

extern volatile Kernel internal_kernel;
extern Kernel volatile *volatile global_kernel;

INLINED uint8_t index_of_current_task(Kernel &kernel) {
  return kernel.schedule.active_task_block;
}

/*
 * Add a task to the task queue. This is needed to let the the task execute.
 * */
uint8_t add_to_task_list(uint8_t priority, Task_Queue &queue);

/*
 * Set up kernel configuration.
 * */
void init_kernel(Kernel &kernel);

/*
 * Set up the scheduler.
 * */
void init_schedule(Kernel &kernel);

/*
 * Initialise heap mapping and canaries.
 */
void init_memory(void);

/*
 * Memory corruption might cause the uC to wrap aroud and re-intiate the
 * starup process.
 * */
void verify_that_kernel_is_uninitilized(Kernel &kernel);

constexpr uint16_t num_context_switch_overhead_bytes(void) {
  constexpr uint16_t num_registers = 32;
  constexpr uint16_t num_pc_bytes = 3;
  constexpr uint16_t num_sreg_bytes = 1;
  return num_pc_bytes + num_registers + num_sreg_bytes;
}

Simplos_Task *get_task(pid_type pid, Kernel &kernel);
Task_Status task_status(pid_type pid, Kernel &kernel);

void kill_current_task(Kernel &kernel);

typedef uint8_t Index;
Index create_simplos_task(const ProgmemString &name, const uint8_t priority,
                          Kernel &kernel);

#endif  // SIMPLOS_H_
