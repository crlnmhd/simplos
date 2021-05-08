#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"

// TODO stack check function.

extern bool debug_queu_is_initialised;
void init_empty_queue(Task_Queue* queue) {
  if (debug_queu_is_initialised) {
    fatal_error("QUEUE APPEARS TO ALREADY BE INITIALISED!");
  }

  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    task->empty = true;
    task->task_memory_block = i;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    task->spawning = false;
  }
  debug_queu_is_initialised = true;
}

// NOT TS
uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    Simplos_Task* task = (Simplos_Task*)&queue->task_queue[i];
    // Take if available
    if (task->empty) {
      cprint("Initiating space for new function at block %d\n", i);
      task->empty = false;
      task->priority = priority;
      task->status = READY;
      return i;
    }
  }
  fatal_error("Can't create new task! Queue if full!");

  // Never reached.
  return 0;
}

void kill_task(uint8_t const task, bool reinable_mt) {
  DISABLE_MT();
  cprint("killing task: %d\n", task);
  Simplos_Task* victim = &simplos_schedule->queue.task_queue[task];
  cprint("Murder victim:");
  print_task(victim, true);
  victim->status = DONE;
  victim->empty = true;
  victim->spawning = false;
  victim->task_sp_adr = task_default_sp(task);
  cprint("Task %d killed\n", task);
  if (reinable_mt) {
    ENABLE_MT();
  }
}

void kill_current_task() {
  DISABLE_MT();
  uint8_t const curr_task_index = simplos_schedule->queue.curr_task_index;
  kill_task(curr_task_index, false);
  k_yield();
}

void k_yield(void) {
  cprint("Yielding\n");
  cli();
  DISABLE_MT();
  TCNT1 = 0;  // FIXME
  context_switch();
  ENABLE_MT();
  if (simplos_schedule->queue
          .task_queue[simplos_schedule->queue.curr_task_index]
          .spawning) {
    simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
        .spawning = false;
    asm volatile(
        "pop __tmp_reg__    \n\t"  // Pop previous callers PC.
        "pop __tmp_reg__    \n\t"  // The sought after PC layes after this.
        "pop __tmp_reg__    \n\t"
        "sei                \n\t"
        "ret                \n\t");
  } else {
    sei();
  }
}

void spawn_task(void (*fn)(void), uint8_t const priority) {
  DISABLE_MT();

  uint8_t const new_task_index =
      add_task_to_queue(priority, &simplos_schedule->queue);

  Simplos_Task* volatile new_task =
      &simplos_schedule->queue.task_queue[new_task_index];

  Simplos_Task* volatile old_task =
      &simplos_schedule->queue
           .task_queue[simplos_schedule->queue.curr_task_index];

  old_task->status = READY;
  old_task->spawning = true;

  simplos_schedule->queue.curr_task_index = new_task_index;

  register uint8_t tmpH, tmpM, tmpL, tmp;
  cli();
  asm volatile(
      "rcall .+0                  \n\t"  // pushes PC (3 bytes) onto the
                                         // stack
      "pop %[tmpH]                \n\t"
      "pop %[tmpM]                \n\t"
      "pop %[tmpL]                \n\t"  // MSB
      "clr __zero_reg__           \n\t"
      "ldi %[tmp], 13             \n\t"
      "add %[tmpL], %[tmp]        \n\t"  // Offset to call OLD_TASK_RET
      "ldi %[tmp], 0              \n\t"  // This may not be nessesary with
                                         // __zero_reg__
      "adc %[tmpM], %[tmp]       \n\t"   // Carry from LSB. tmp = 0
      "adc %[tmpH], %[tmp]       \n\t"   // Carry from Middle byte
      "push %[tmpL]              \n\t"   // PUSH LS byte of PC back onto the
                                         // stack.
      "push %[tmpM]              \n\t"
      "push %[tmpH]              \n\t"
      "jmp CONTINUE              \n\t"  // First run hits this.
      "nop                       \n\t"  // When returning from context switch
                                        // after spawning.
      "ret                       \n\t"
      "CONTINUE:                 \n\t"
      " nop                      \n\t"
      : [tmpH] "=r"(tmpH), [tmpM] "=r"(tmpM), [tmpL] "=r"(tmpL), [tmp] "=r"(tmp)
      :);
  sei();
  SAVE_CONTEXT();
  SAVE_SP();
  cprint("Setting task %d sp adr to 0x%X\n", old_task->task_memory_block,
         *task_sp);
  old_task->task_sp_adr = *task_sp;

  *task_sp = new_task->task_sp_adr;
  SET_SP();

  cprint("Calling function\n");
  ENABLE_MT();
  fn();
  DISABLE_MT();
  kill_current_task();
  asm volatile(
      "exit_lbl:        \n\t"
      " nop             \n\t");
}
