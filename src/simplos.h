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

void kill_task(uint8_t const, bool);
void kill_current_task(void);

void k_yield(void) __attribute__((naked));

// More or less borrowed from
// https://www.freertos.org/kernel/secondarydocs.html

#define SAVE_CONTEXT()                    \
  asm volatile(                           \
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
      "sei                          \n\t");

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

#define RESTORE_CONTEXT()                \
  asm volatile(                          \
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

// void create_task(void (*fn)(void), uint8_t priority, Task_Queue* task_queue);
void init_empty_queue(Task_Queue*);

// Must be run when multitasking is off

// #define curr_task
// simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]

#define PUSH_PC() asm volatile("rcall .+0")

#define POP_PC() \
  *task_sp -= 3; \
  SET_SP();

// Used to "return" to a new function when spawning.

// Calls fn after yieldning. Usefull for spawning task.
void call_fn_fn(void (*fn)(void));

void spawn_task(void (*fn)(void), uint8_t const priority);
// INLINED
// void spawn_task(void (*fn)(void), uint8_t const priority) {
//   DISABLE_MT();
//   uint8_t const new_task_index =
//       add_task_to_queue(priority, &simplos_schedule->queue);

//   Simplos_Task* new_task =
//   &simplos_schedule->queue.task_queue[new_task_index];

//   size_t volatile* volatile newtask_sp =
//       (size_t volatile*)new_task->task_sp_adr;
//   // Call will push 3 bytes to stack starging at task_sp_adr
//   SAVE_SP();
//   asm volatile(
//       "lds r16, %0                 \n\t"
//       "lds r17, %1                 \n\t"
//       "lds r18, %2                 \n\t"
//       "lds r26, %3                 \n\t"
//       "lds r27, %4                 \n\t"
//       "ld  r28, x+                 \n\t"
//       "out  __SP_L__, r28          \n\t"
//       "ld   r29, x+                \n\t"
//       "out  __SP_H__, r29          \n\t"
//       "push r18                    \n\t"
//       "push r17                    \n\t"
//       "push r26                    \n\t"
//       "lds  r26, task_sp           \n\t"
//       "lds  r27, task_sp + 1       \n\t"
//       "ld   r28, x+                \n\t"
//       "out  __SP_L__, r28          \n\t"
//       "ld   r29, x+                \n\t"
//       "out  __SP_H__, r29          \n\t" ::"M"(fn),
//       "M"(fn + 1), "M"(fn + 2), "M"(newtask_sp), "M"(newtask_sp + 1)
//       : "memory");

//   new_task->empty = false;
//   new_task->priority = priority;
//   dprint("DONE preparing up new task");
//   k_yield();
// }

// // NO MT
// INLINED
// void spawn_task(void (*fn)(void), uint8_t const priority) {
//   DISABLE_MT();
//   uint8_t const new_task_index =
//       add_task_to_queue(priority, &simplos_schedule->queue);

//   Simplos_Task* new_task =
//   &simplos_schedule->queue.task_queue[new_task_index];

//   volatile Simplos_Task* old_task =
//       &simplos_schedule->queue
//            .task_queue[simplos_schedule->queue.curr_task_index];
//   printf("old task index in spawn_task(): %d\n",
//          simplos_schedule->queue.curr_task_index);

//   old_task->spawning = true;

//   old_task->status = READY;
//   old_task->task_sp_adr = *task_sp;

//   simplos_schedule->queue.curr_task_index = new_task_index;
//   printf("Set curr task to new task = %d == %d\n", new_task_index,
//          simplos_schedule->queue.curr_task_index);

//   PUSH_PC();

//   SAVE_CONTEXT();
//   SAVE_SP();

//   // Return point.
//   if (simplos_schedule->queue
//           .task_queue[simplos_schedule->queue.curr_task_index]
//           .spawning) {
//     SET_SP();
//     RESTORE_CONTEXT();
//     // POP_PC();
//     simplos_schedule->queue.task_queue[simplos_schedule->queue.curr_task_index]
//         .spawning = false;
//     dprint("Restoring calling task by returning from spawn_task()\n");
//     return;
//   }

//   *task_sp = new_task->task_sp_adr;
//   SET_SP();

//   dprint("Running function\n");
//   ENABLE_MT();
//   fn();
//   DISABLE_MT();
//   /*
//     WARNING, the stack is still that of the new task. Here be dragons!
//   */
//   // *task_sp = old_task->task_sp_adr;
//   // SET_SP();

//   dprint("Task done!\n");
//   kill_current_task();
// }

INLINED
void save_running_task(void) {
  INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  SAVE_CONTEXT();
  SAVE_SP();

  Simplos_Task* prev =
      &simplos_schedule->queue
           .task_queue[simplos_schedule->queue.curr_task_index];
  prev->task_sp_adr = *task_sp;
  prev->status = READY;
  cprint("saving task %d's SP 0x%X\n", prev->task_memory_block,
         prev->task_sp_adr);
}

INLINED
void prepare_next_task(Simplos_Task* task) {
  cprint("Switching to new task: ");

  INTERNAL_LED_PORT &= ~(1 << INTERNAL_LED);
  task->status = RUNNING;
  // If the thread was spawning a new thread it is now ready to continue.
  // task->spawning = false;

  *task_sp = task->task_sp_adr;
  print_task(task, true);
  SET_SP();
  RESTORE_CONTEXT();
}

INLINED
void context_switch(void) {
  cprint("############## Context switch ##############\n");
  save_running_task();

  uint8_t const task_nr = select_next_task();  // Updates curr_task_index
  Simplos_Task* new_task = &simplos_schedule->queue.task_queue[task_nr];
  prepare_next_task(new_task);
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