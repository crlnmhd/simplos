#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hal.h"
#include "io_helpers.h"
#include "memory.h"
#include "memory_layout.h"
#include "timers.h"

NO_MT void init_task_list(Task_Queue *queue) {
  check_task_configuration_uses_all_available_memory();

  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_type task = (taskptr_type)&queue->tasks[i];
    task->task_memory_block = i;
    task->time_counter = 0;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    cprint("Initiating mem block %d at 0x%X-0x%X\n", i,
           task->task_sp_adr - task_memory_size(), task->task_sp_adr);
    task->status = EMPTY;
  }
}

NO_MT uint8_t add_to_task_list(uint8_t priority, Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_type task = (Simplos_Task *)&queue->tasks[i];
    // Take if available
    if (task->status == EMPTY) {
#if defined(VERBOSE_OUTPUT)
      cprint("Initiating space for new function at block %d\n", i);
#endif  // VERBOSE_OUTPUT
      task->priority = priority;
      task->status = READY;
      task->task_sp_adr = task_default_sp(task->task_memory_block);
      task->pid = UINT16_MAX;
      return i;
    }
  }
  FATAL_ERROR("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

NO_MT void init_schedule(void) {
  kernel->schedule.os_task_sp = OS_STACK_START;
  init_task_list(&(kernel->schedule.queue));
  kernel->cs_time_counter = 0;
}

__attribute__((noinline, naked)) void k_yield(void) {
  asm volatile("cli" ::: "memory");
  CONTEXT_SWTICH();
  asm volatile(
      "sei  \n\t"
      "ret  \n\t"
      : /* No outputs*/
      : /* No input*/
      : "memory");
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  CONTEXT_SWTICH();
  reti();
}

NO_MT void init_kernel(void) {
  kernel->schedule.queue.queue_position = 0;
  for (uint8_t i = 0; i < TASKS_MAX; i++) {
    // Empty task name.
    set_task_name(i, "");

    // Set task RAM range.
    BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
    struct StackRange *task_stack_range = &kernel->task_RAM_ranges[i];
    END_DISCARD_VOLATILE_QUALIFIER_WARNING()
    task_stack_range->high = task_default_sp(i);
    task_stack_range->low = task_default_sp(i) - task_memory_size();
  }
}

pid_type spawn_task(void (*fn)(void), uint8_t const priority,
                    char const *name) {
  DISABLE_MT();
  uint8_t const new_task_index = create_simplos_task(name, priority);

  Simplos_Task *new_task = &kernel->schedule.queue.tasks[new_task_index];
  new_task->status = RUNNING;

  uint16_t const new_task_pid = new_task->pid;

  taskptr_type old_task = &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK];

  old_task->status = READY;

#if defined(SW_TIME_MEASSREMENTS)
  old_task->time_counter += GET_TICK_COUNTER();
  RESET_TICK_COUNTER();
#endif  // SW_TIME_MEASSREMNTS
  INDEX_OF_CURRENT_TASK = new_task_index;

  cli();
  asm goto(
      "rcall .+0            \n\t"  // pushes PC (3 bytes) onto the
      // stack
      "pop r16              \n\t"
      "pop r17              \n\t"
      "pop r18              \n\t"  // MSB
      "ldi r19, 12          \n\t"
      "add r18, r19         \n\t"  // Offset to jmp RETPOINT
      "ldi r19, 0           \n\t"  // Clearing the registe might not be
                                   // nessesary.
      "adc r17, r19         \n\t"  // Carry from LSB.
      "adc r16, r19         \n\t"  // Carry from Middle byte
      "push r18             \n\t"  // PUSH LS byte of PC back onto the
      // stack.
      "push r17             \n\t"
      "push r16             \n\t"
      "cpse r1, r1          \n\t"  // Skip next instruction
      "jmp %l[return_point] \n\t"  // Target
      "nop                  \n\t"
      : /* No outputs */
      : /* No inputs */
      : /* No clobbers */
      : return_point);

  SAVE_CONTEXT();

  sei();
  old_task->task_sp_adr = task_sp;

  task_sp = kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].task_sp_adr;
  SET_SP();
  cprint("Calling function\n");
  ENABLE_MT();
  fn();
  DISABLE_MT();
  cprint("Function finnished\n");
  kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK].status = EMPTY;
  kernel->task_names[INDEX_OF_CURRENT_TASK][0] = '\0';

  k_yield();  // re-enable interrupts.

  cprint("Task killed");

return_point:
  return new_task_pid;
}

inline void set_task_name(const Index task_index, const char *name) {
  strlcpy((char *)kernel->task_names[task_index], name,
          FUNCTION_NAME_MAX_LENGTH + 1);
}

inline Index create_simplos_task(const char *name, const uint8_t priority) {
  uint8_t const index = add_to_task_list(priority, &kernel->schedule.queue);
  Simplos_Task *new_task = &kernel->schedule.queue.tasks[index];
  new_task->status = RUNNING;
  new_task->pid = kernel->pid_cnt++;
  set_task_name(index, name);

#if defined(DEBUG_OUTPUT)
  cprint("Created simplos task %s with pid %d\n", name, new_task->pid);
#endif  // DEBUG_OUTPUT
  return index;
}

uint16_t num_context_switch_overhead_bytes(void) {
  const uint16_t num_registers = 32;
  const uint16_t num_pc_bytes = 3;
  const uint16_t num_sreg_bytes = 1;
  return num_pc_bytes + num_registers + num_sreg_bytes;
}

void kill_current_task(void) {
  cli();
  ENABLE_MT();

  uint8_t const curr_task_index = INDEX_OF_CURRENT_TASK;

  Simplos_Task *task = &kernel->schedule.queue.tasks[curr_task_index];
  task->status = EMPTY;
  kernel->task_names[curr_task_index][0] = '\0';
  kernel->ended_task_time_counter += task->time_counter;
  k_yield();  // re-enables interrupts.
}

Simplos_Task *get_task(pid_type pid) {
  DISABLE_MT();
  for (uint8_t t = 0; t < TASKS_MAX; ++t) {
    if (kernel->schedule.queue.tasks[t].pid == pid) {
      return &kernel->schedule.queue.tasks[t];
    }
  }
  ENABLE_MT();
  return NULL;
}

enum Task_Status task_status(pid_type pid) {
  taskptr_type task = get_task(pid);
  if (task == NULL) {
    return EMPTY;
  }
  return task->status;
}

void init_memory(void) {
  for (uint8_t *canary_adr = (uint8_t *)CANARY_START;
       (size_t)canary_adr >= CANARY_END; canary_adr--) {
    write_mm(canary_adr, CANARY_VALUE);
  }
  ASSERT_EQ(*(uint8_t *)0x204, CANARY_VALUE, "0x%X",
            "Failed to write register-os stack canary!");
  ASSERT_EQ(*(uint8_t *)0x203, CANARY_VALUE, "0x%X",
            "Failed to write register-os stack canary!");
  ASSERT_EQ(*(uint8_t *)0x202, CANARY_VALUE, "0x%X",
            "Failed to write register-os stack canary!");
  ASSERT_EQ(*(uint8_t *)0x201, CANARY_VALUE, "0x%X",
            "Failed to write register-os stack canary!");
  ASSERT_EQ(*(uint8_t *)0x200, CANARY_VALUE, "0x%X",
            "Failed to write register-os stack canary!");
  /*
  for (uint16_t i = 0; i < HEAP_PAGE_SIZE; ++i) {
    kernel->heap_mapping[i] = 0xFF;
  }
  return 0;
  */
}
