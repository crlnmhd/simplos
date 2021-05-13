#ifndef SIMPLOS_H_
#define SIMPLOS_H_

// #define  STACK_POINTER ((volatile uint16_t * const) 0x5E)

#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "interupts.h"
#include "io_helpers.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos_types.h"
#include "tasks.h"

extern uint16_t volatile* volatile task_sp;
extern volatile uint16_t _task_sp_adr;
extern Scheduler volatile* volatile simplos_schedule;

#define INLINED static inline __attribute__((always_inline))

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5

uint8_t add_task_to_queue(uint8_t priority, Task_Queue* queue);
void init_empty_queue(Task_Queue*);

// INLINED
// void kill_current_task(void);

// void kill_task(uint8_t const, bool);
// void kill_current_task(void);

__attribute__((naked, noinline)) void k_yield(void);

// More or less borrowed from
// https://www.freertos.org/kernel/secondarydocs.html

#define SAVE_CONTEXT()                    \
  asm volatile(                           \
      "cli                         \n\t"  \
      "push  r0                    \n\t"  \
      "in    r0, __SREG__          \n\t"  \
      "cli                         \n\t"  \
      "push  r0                    \n\t"  \
      "push  r1                    \n\t"  \
      "clr   r1                    \n\t"  \
      "push  r2                    \n\t"  \
      "push  r3                    \n\t"  \
      "push  r4                    \n\t"  \
      "push  r5                    \n\t"  \
      "push  r6                    \n\t"  \
      "push  r7                    \n\t"  \
      "push  r8                    \n\t"  \
      "push  r9                    \n\t"  \
      "push  r10                   \n\t"  \
      "push  r11                   \n\t"  \
      "push  r12                    \n\t" \
      "push  r13                    \n\t" \
      "push  r14                    \n\t" \
      "push  r15                    \n\t" \
      "push  r16                    \n\t" \
      "push  r17                    \n\t" \
      "push  r18                    \n\t" \
      "push  r19                    \n\t" \
      "push  r20                    \n\t" \
      "push  r21                    \n\t" \
      "push  r22                    \n\t" \
      "push  r23                    \n\t" \
      "push  r24                    \n\t" \
      "push  r25                    \n\t" \
      "push  r26                    \n\t" \
      "push  r27                    \n\t" \
      "push  r28                    \n\t" \
      "push  r29                    \n\t" \
      "push  r30                    \n\t" \
      "push  r31                    \n\t" \
      "lds   r26, task_sp           \n\t" \
      "lds   r27, task_sp + 1       \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t" \
      "sei                          \n\t");

// INLINED uint16_t SAVE_SP(void) { return *((uint16_t volatile*)(0x5E)); }

// #define SET_SP() ;

// #define SP *((uint16_t volatile*)(0x5E))

#define SET_SP() SP = *task_sp;
#define SAVE_SP() *task_sp = SP;

/*
 #define SAVE_SP()                         \
   asm volatile(                           \
      "lds   r26, task_sp           \n\t" \
      "lds   r27, task_sp + 1       \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t");

#define SET_SP()                         \
  asm volatile(                          \
      "lds  r26, task_sp           \n\t" \
      "lds  r27, task_sp + 1       \n\t" \
      "ld   r28, x+                \n\t" \
      "out  __SP_L__, r28          \n\t" \
      "ld   r29, x+                \n\t" \
      "out  __SP_H__, r29          \n\t");
      */

#define RESTORE_CONTEXT()                \
  asm volatile(                          \
      "lds  r26, task_sp           \n\t" \
      "lds  r27, task_sp + 1       \n\t" \
      "ld   r28, x+                \n\t" \
      "out  __SP_L__, r28          \n\t" \
      "ld   r29, x+                \n\t" \
      "out  __SP_H__, r29          \n\t" \
      "pop  r31                    \n\t" \
      "pop  r30                    \n\t" \
      "pop  r29                    \n\t" \
      "pop  r28                    \n\t" \
      "pop  r27                    \n\t" \
      "pop  r26                    \n\t" \
      "pop  r25                    \n\t" \
      "pop  r24                    \n\t" \
      "pop  r23                    \n\t" \
      "pop  r22                    \n\t" \
      "pop  r21                    \n\t" \
      "pop  r20                    \n\t" \
      "pop  r19                    \n\t" \
      "pop  r18                    \n\t" \
      "pop  r17                    \n\t" \
      "pop  r16                    \n\t" \
      "pop  r15                    \n\t" \
      "pop  r14                    \n\t" \
      "pop  r13                    \n\t" \
      "pop  r12                    \n\t" \
      "pop  r11                    \n\t" \
      "pop  r10                    \n\t" \
      "pop  r9                     \n\t" \
      "pop  r8                     \n\t" \
      "pop  r7                     \n\t" \
      "pop  r6                     \n\t" \
      "pop  r5                     \n\t" \
      "pop  r4                     \n\t" \
      "pop  r3                     \n\t" \
      "pop  r2                     \n\t" \
      "pop  r1                     \n\t" \
      "pop  r0                     \n\t" \
      "out  __SREG__, r0           \n\t" \
      "pop  r0                     \n\t");

INLINED void save_running_task(void) {
  // INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  SAVE_CONTEXT();
  // cprint("############## Context switch ##############\n");

  Simplos_Task* prev =
      &simplos_schedule->queue
           .task_queue[simplos_schedule->queue.curr_task_index];
  if (prev->status == EMPTY) {
    // The previous task has been killed.
    return;
  }
  prev->task_sp_adr = *task_sp;
  prev->status = READY;
  // cprint("saving task %d's SP 0x%X\n", prev->task_memory_block,
  //        prev->task_sp_adr);
}

INLINED
void prepare_next_task(uint8_t const task_nr) {
  INTERNAL_LED_PORT &= ~(1 << INTERNAL_LED);
  Simplos_Task* volatile task = &simplos_schedule->queue.task_queue[task_nr];
  task->status = RUNNING;
  // If the thread was spawning a new thread it is now ready to continue.
  // task->spawning = false;
  *task_sp = task->task_sp_adr;
  RESTORE_CONTEXT();
  ENABLE_MT();
}

INLINED
void context_switch(void) {
  save_running_task();
  uint8_t const task_nr = select_next_task();  // Updates curr_task_index
  // cprint("Switching to task : %d\n", task_nr);
  prepare_next_task(task_nr);
  // cprint("New task sp is 0x%X\n",
  //        simplos_schedule->queue
  //            .task_queue[simplos_schedule->queue.curr_task_index]
  //            .task_sp_adr);
  // SAVE_SP();
  // cprint("Task sp is : 0x%X\n", *task_sp);

  // cprint("~~~~~~~~~~~~~~ contect switch done ~~~~~~~~~~~~~\n");
}

// INLINED
// void k_yield(void) {
//   context_switch();
//   TCNT1 = 0;
//   // TIMER1_COMPA_vect();
// }

INLINED
void kill_current_task(void) {
  cli();
  ENABLE_MT();
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .status = EMPTY;
  k_yield();  // reinables interupts.
}

INLINED
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
  // old_task->spawning = true;

  new_task->status = RUNNING;

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
      "jmp CONTINUE              \n\t"  // First run hits this.
      "nop                       \n\t"  // Aim for this. When returning from
                                        // context switch after spawning.
      "jmp RETPOINT              \n\t"
      "CONTINUE:                 \n\t"
      : [tmpH] "=r"(tmpH), [tmpM] "=r"(tmpM), [tmpL] "=r"(tmpL), [tmp] "=r"(tmp)
      :);
  sei();

  SAVE_CONTEXT();
  cprint("Setting task %d sp adr to 0x%X\n", old_task->task_memory_block,
         *task_sp);
  old_task->task_sp_adr = *task_sp;

  *task_sp = new_task->task_sp_adr;
  SET_SP();

  cprint("Calling function\n");
  ENABLE_MT();
  fn();
  DISABLE_MT();
  cprint("Function finnished\n");
  simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
      .status = EMPTY;

  k_yield();  // reinables interupts.

  cprint("Task killed");
  asm volatile(
      "RETPOINT:        \n\t"
      " nop             \n\t");
}

// INLINED
// void yield(void) {
//   // ATOMIC_BLOCK(ATOMIC_FORCEON) { TIMER1_COMPA_vect(); }
//   // "Reset" timer
//   // TCNT1 = TIMER_COMPARE_MATCH;  // FIXME
//   // PUSH_PC();
//   TCNT1 = 0;
//   test_isr()
// }
// context_switch();
// asm volatile("ret");

//   // asm volatile("call 0xE");
//   // TIMER0_COMPA_vect();
//   // asm volatile()
//   // ENABLE_MT();
//   // asm volatile("ret");
// }

#endif  // SIMPLOS_H_