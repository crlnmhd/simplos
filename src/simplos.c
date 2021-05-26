#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"

// TODO stack check function.

void init_empty_queue(Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_t task = (Simplos_Task*)&queue->task_queue[i];
    task->task_memory_block = i;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    cprint("Initiating mem block %d with SP 0x%X\n", i, task->task_sp_adr);
    task->status = EMPTY;
  }
}

// NOT TS
uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_t task = (Simplos_Task*)&queue->task_queue[i];
    // Take if available
    if (task->status == EMPTY) {
      cprint("Initiating space for new function at block %d\n", i);
      task->priority = priority;
      task->status = READY;
      task->task_sp_adr = task_default_sp(task->task_memory_block);
      return i;
    }
  }
  fatal_error("Can't create new task! Queue if full!");

  return 0;  // Never reached
}

void init_schedule(void) {
  *simplos_schedule->os_task_sp = os_stack_start();
  init_empty_queue(&simplos_schedule->queue);
}

// void kill_task(uint8_t const task, bool reinable_mt) {
//   DISABLE_MT();
//   cprint("killing task: %d\n", task);
//   Simplos_Task* victim = &simplos_schedule->queue.task_queue[task];
//   cprint("Murder victim:");
//   victim->status = EMPTY;
//   print_task(victim, true);
//   victim->task_sp_adr = task_default_sp(task);
//   cprint("Task %d killed\n", task);
//   if (reinable_mt) {
//     ENABLE_MT();
//   }
// }

// void kill_current_task() {
//   DISABLE_MT();
//   uint8_t const curr_task_index = simplos_schedule->queue.curr_task_index;
//   kill_task(curr_task_index, false);
//   // k_yield();  // enables MT.
//   uint8_t const task_nr = select_next_task();  // Updates curr_task_index
//   Simplos_Task* new_task = &simplos_schedule->queue.task_queue[task_nr];
//   prepare_next_task(new_task);
// }

__attribute__((noinline, naked)) void k_yield(void) {
  cli();
  // INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  // PORTB = 0xFF;  // turn on led, and whatever else happen to be there...
  context_switch();
  // asm volatile("ret" ::: "memory");
  sei();
  asm volatile("ret");
}

// Timer interupt for context switching
ISR(TIMER1_COMPA_vect, ISR_NAKED) {
  context_switch();
  reti();
}

__attribute__((noinline)) void spawn_task(void (*fn)(void),
                                          uint8_t const priority) {
  cprint("Spawning task\n");
  DISABLE_MT();

  uint8_t const new_task_index =
      add_task_to_queue(priority, &simplos_schedule->queue);

  // Simplos_Task* volatile new_task =
  simplos_schedule->queue.task_queue[new_task_index].status = RUNNING;

  taskptr_t old_task =
      &simplos_schedule->queue
           .task_queue[simplos_schedule->queue.curr_task_index];
  // .status = READY;

  old_task->status = READY;
  // old_task->spawning = true;

  // new_task->status = RUNNING;

  simplos_schedule->queue.curr_task_index = new_task_index;

  register uint8_t tmpH, tmpM, tmpL, tmp;
  cli();
  asm volatile(
      "rcall .+0                  \n\t"  // pushes PC (3 bytes) onto the
                                         // stack
      "pop %[tmpH]                \n\t"
      "pop %[tmpM]                \n\t"
      "pop %[tmpL]                \n\t"  // MSB
      "ldi %[tmp], 12             \n\t"
      "add %[tmpL], %[tmp]        \n\t"  // Offset to call OLD_TASK_RET
      "ldi %[tmp], 0              \n\t"  // This may not be nessesary with
                                         // __zero_reg__
      "adc %[tmpM], %[tmp]       \n\t"   // Carry from LSB. tmp = 0
      "adc %[tmpH], %[tmp]       \n\t"   // Carry from Middle byte
      "push %[tmpL]              \n\t"   // PUSH LS byte of PC back onto the
                                         // stack.
      "push %[tmpM]              \n\t"
      "push %[tmpH]              \n\t"
      "cpse r1, r1               \n\t"  // Skip next instruction (avoids
                                        // creating a label)
      "jmp RETPOINT                       \n\t"
      "nop                       \n\t"
      :
      [tmpH] "=r"(tmpH), [tmpM] "=r"(tmpM), [tmpL] "=r"(tmpL), [tmp] "=r"(tmp));
  sei();

  SAVE_CONTEXT();
  // cprint("Setting task %d sp adr to 0x%X\n", old_task->task_memory_block,
  //        *task_sp);

  old_task->task_sp_adr = *task_sp;
  // Simplos_Task* volatile new_task =
  //     &simplos_schedule->queue
  //          .task_queue[simplos_schedule->queue.curr_task_index];

  *task_sp = simplos_schedule->queue
                 .task_queue[simplos_schedule->queue.curr_task_index]
                 .task_sp_adr;
  SET_SP();

  // cprint("Before calling function\n");
  // print_schedule();
  cprint("Calling function\n");
  ENABLE_MT();
  fn();
  DISABLE_MT();
  cprint("Function finnished\n");
  // cprint("Setting task %d to empty\n",
  // simplos_schedule->queue.curr_task_index);
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .status = EMPTY;

  // cprint("Before yielding : \n");
  // print_schedule();

  k_yield();  // reinables interupts.

  cprint("Task killed");

  asm volatile(
      "RETPOINT:        \n\t"
      " nop             \n\t");
}

// void kill_current_task(void) {
//   ENABLE_MT();
//   cli();
//   simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
//       .status = EMPTY;
//   k_yield();  // reinables interupts.
// }