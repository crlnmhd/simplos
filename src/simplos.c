#include "simplos.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <util/delay.h>

#include "io_helpers.h"
#include "memory.h"

void init_empty_queue(Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_t task = (taskptr_t)&queue->task_queue[i];
    task->task_memory_block = i;
    task->task_sp_adr = task_default_sp(task->task_memory_block);
    cprint("Initiating mem block %d with SP 0x%X\n", i, task->task_sp_adr);
    task->status = EMPTY;
  }
}

// NOT TS
uint8_t add_task_to_queue(uint8_t priority, Task_Queue *queue) {
  for (uint8_t i = 0; i < TASKS_MAX; ++i) {
    taskptr_t task = (Simplos_Task *)&queue->task_queue[i];
    // Take if available
    if (task->status == EMPTY) {
      cprint("Initiating space for new function at block %d\n", i);
      task->priority = priority;
      task->status = READY;
      task->task_sp_adr = task_default_sp(task->task_memory_block);
      task->pid = UINT16_MAX;
      return i;
    }
  }
  fatal_error("Can't create new task! Queue if full!");

  return 0; // Never reached
}

void init_schedule(void) {
  simplos_schedule->os_task_sp = os_stack_start();
  init_empty_queue(&simplos_schedule->queue);
}

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
  PORTA |= (1 << PORTA0);
  context_switch();
  PORTA &= ~(1 << PORTA0);
  reti();
}

pid_t spawn_task(void (*fn)(void), uint8_t const priority) {
  DISABLE_MT();

  uint8_t const new_task_index =
      add_task_to_queue(priority, &simplos_schedule->queue);

  // Simplos_Task* volatile new_task =
  simplos_schedule->queue.task_queue[new_task_index].status = RUNNING;
  uint16_t const new_task_pid = pid_cnt++;
  cprint("Spawning task with PID %d\n", new_task_pid);
  simplos_schedule->queue.task_queue[new_task_index].pid = new_task_pid;

  taskptr_t old_task =
      &simplos_schedule->queue
           .task_queue[simplos_schedule->queue.curr_task_index];
  // .status = READY;

  old_task->status = READY;

  simplos_schedule->queue.curr_task_index = new_task_index;

  register uint8_t tmpH, tmpM, tmpL, tmp;
  cli();
  asm volatile(
      "rcall .+0                  \n\t" // pushes PC (3 bytes) onto the
                                        // stack
      "pop %[tmpH]                \n\t"
      "pop %[tmpM]                \n\t"
      "pop %[tmpL]                \n\t" // MSB
      "ldi %[tmp], 12             \n\t"
      "add %[tmpL], %[tmp]        \n\t" // Offset to call OLD_TASK_RET
      "ldi %[tmp], 0              \n\t" // This may not be nessesary with
                                        // __zero_reg__
      "adc %[tmpM], %[tmp]       \n\t"  // Carry from LSB. tmp = 0
      "adc %[tmpH], %[tmp]       \n\t"  // Carry from Middle byte
      "push %[tmpL]              \n\t"  // PUSH LS byte of PC back onto the
                                        // stack.
      "push %[tmpM]              \n\t"
      "push %[tmpH]              \n\t"
      "cpse r1, r1               \n\t" // Skip next instruction
      "jmp RETPOINT              \n\t"
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

  k_yield(); // reinables interupts.

  cprint("Task killed");

  asm volatile("RETPOINT:        \n\t"
               " nop             \n\t");
  return new_task_pid;
}

void kill_current_task(void) {
  cli();
  ENABLE_MT();
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .status = EMPTY;
  k_yield(); // reinables interupts.
}

Simplos_Task *get_task(pid_t pid) {
  DISABLE_MT();
  for (uint8_t t = 0; t < TASKS_MAX; ++t) {
    if (simplos_schedule->queue.task_queue[t].pid == pid) {
      return &simplos_schedule->queue.task_queue[t];
    }
  }
  ENABLE_MT();
  return NULL;
}

enum Task_Status task_status(pid_t pid) {
  taskptr_t task = get_task(pid);
  if (task == NULL) {
    return EMPTY;
  }
  return task->status;
}
