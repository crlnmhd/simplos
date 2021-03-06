#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
//#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"
#include "memory_layout.h"
#include "timers.h"

NO_MT void init_empty_queue(Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_type task = (taskptr_type)&queue->task_queue[i];
    task->task_memory_block = i;
    task->time_counter = 0;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
    char *task_name_buf = kernel->task_names[i];
    END_DISCARD_VOLATILE_QUALIFIER_WARNING()
    strlcpy(task_name_buf, "", FUNCTION_NAME_MAX_LENGTH + 1);
    cprint("Initiating mem block %d with SP 0x%X\n", i, task->task_sp_adr);
    task->status = EMPTY;
  }
}

NO_MT uint8_t add_task_to_queue(uint8_t priority, Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_type task = (Simplos_Task *)&queue->task_queue[i];
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
  fatal_error("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

NO_MT void init_schedule(void) {
  kernel->schedule.os_task_sp = OS_STACK_START;
  init_empty_queue(&(kernel->schedule.queue));
  kernel->cs_time_counter = 0;
}

__attribute__((noinline, naked)) void k_yield(void) {
  cli();
  // TODO do blinking in asm.
  // INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  // PORTB = 0xFF;  // turn on led, and whatever else happen to be there...

  context_switch();
  // is handled manually).
  // asm volatile("ret" ::: "memory");
  sei();
  asm volatile("ret");
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  // PORTA |= (1 << PORTA0);
  context_switch();
  // PORTA &= ~(1 << PORTA0);
  reti();
}

pid_type spawn_task(void (*fn)(void), uint8_t const priority,
                    char const *name) {
  DISABLE_MT();
  uint8_t const new_task_index =
      add_task_to_queue(priority, &kernel->schedule.queue);

  kernel->schedule.queue.task_queue[new_task_index].status = RUNNING;
  BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
  char *task_name_buffer = kernel->task_names[new_task_index];
  END_DISCARD_VOLATILE_QUALIFIER_WARNING()
  strlcpy(task_name_buffer, name, FUNCTION_NAME_MAX_LENGTH + 1);

  uint16_t const new_task_pid = kernel->pid_cnt++;
  kernel->schedule.queue.task_queue[new_task_index].pid = new_task_pid;
  cprint("Spawning task \"%s\" with PID %d\n", task_name_buffer, new_task_pid);
  taskptr_type old_task =
      &kernel->schedule.queue
           .task_queue[kernel->schedule.queue.curr_task_index];

  old_task->status = READY;

#if defined(SW_TIME_MEASSREMENTS)
  old_task->time_counter += GET_TICK_COUNTER();
  RESET_TICK_COUNTER();
#endif  // SW_TIME_MEASSREMNTS
  kernel->schedule.queue.curr_task_index = new_task_index;

  register uint8_t tmpH, tmpM, tmpL, tmp;
  cli();
  asm volatile(
      "rcall .+0                  \n\t"  // pushes PC (3 bytes) onto the
      // stack
      "pop %[tmpH]                \n\t"
      "pop %[tmpM]                \n\t"
      "pop %[tmpL]                \n\t"  // MSB
      "ldi %[tmp], 12             \n\t"
      "add %[tmpL], %[tmp]        \n\t"  // Offset to jmp RETPOINT
      "ldi %[tmp], 0              \n\t"  // This may not be nessesary with
      // __zero_reg__
      "adc %[tmpM], %[tmp]       \n\t"  // Carry from LSB. tmp = 0
      "adc %[tmpH], %[tmp]       \n\t"  // Carry from Middle byte
      "push %[tmpL]              \n\t"  // PUSH LS byte of PC back onto the
      // stack.
      "push %[tmpM]              \n\t"
      "push %[tmpH]              \n\t"
      "cpse r1, r1               \n\t"  // Skip next instruction
      "jmp RETPOINT              \n\t"
      "nop                       \n\t"
      :
      [tmpH] "=r"(tmpH), [tmpM] "=r"(tmpM), [tmpL] "=r"(tmpL), [tmp] "=r"(tmp));
  sei();

  SAVE_CONTEXT();

  old_task->task_sp_adr = *task_sp;

  *task_sp =
      kernel->schedule.queue.task_queue[kernel->schedule.queue.curr_task_index]
          .task_sp_adr;
  SET_SP();
  cprint("Calling function\n");
  ENABLE_MT();
  fn();
  DISABLE_MT();
  cprint("Function finnished\n");
  kernel->schedule.queue.task_queue[kernel->schedule.queue.curr_task_index]
      .status = EMPTY;
  kernel->task_names[kernel->schedule.queue.curr_task_index][0] = '\0';

  k_yield();  // re-enable interrupts.

  cprint("Task killed");

  asm volatile(
      "RETPOINT:        \n\t"
      " nop             \n\t");
  return new_task_pid;
}

void kill_current_task(void) {
  cli();
  ENABLE_MT();

  uint8_t const curr_task_index = kernel->schedule.queue.curr_task_index;

  Simplos_Task *task = &kernel->schedule.queue.task_queue[curr_task_index];
  task->status = EMPTY;
  kernel->task_names[curr_task_index][0] = '\0';
  kernel->ended_task_time_counter += task->time_counter;
  k_yield();  // re-enables interrupts.
}

Simplos_Task *get_task(pid_type pid) {
  DISABLE_MT();
  for (uint8_t t = 0; t < TASKS_MAX; ++t) {
    if (kernel->schedule.queue.task_queue[t].pid == pid) {
      return &kernel->schedule.queue.task_queue[t];
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
    *canary_adr = CANARY_VALUE;
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
