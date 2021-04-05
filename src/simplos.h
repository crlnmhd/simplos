#ifndef SIMPLOS_H_
#define SIMPLOS_H_

// #define  STACK_POINTER ((volatile uint16_t * const) 0x5E)

#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "scheduler.h"
#include "simplos_types.h"
#include "tasks.h"

#define ENABLE_MT()        \
  TIMSK1 |= (1 << OCIE2A); \
  sei();                   \
  printf("enabling MT\n");

#define DISABLE_MT()        \
  TIMSK1 &= ~(1 << OCIE2A); \
  printf("disabling MT\n");

extern volatile uint16_t task_sp;
extern volatile Scheduler simplos_schedule;

#define INLINED static inline __attribute__((always_inline))

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5

uint8_t add_task_to_queue(uint8_t priority, volatile Task_Queue* queue);

void kill_task(volatile Scheduler*, uint8_t);

uint16_t task_default_sp(uint8_t);

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
      "push  r31                    \n\t");

#define SAVE_SP()                             \
  asm volatile(                               \
      "lds   r26, task_sp               \n\t" \
      "lds   r27, task_sp + 1           \n\t" \
      "in    r0, __SP_L__           \n\t"     \
      "st    x+, r0                 \n\t"     \
      "in    r0, __SP_H__           \n\t"     \
      "st    x+, r0                 \n\t");

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

#define SET_SP()                             \
  asm volatile(                              \
      "lds  r26, task_sp               \n\t" \
      "lds  r27, task_sp + 1           \n\t" \
      "ld   r28, x+                \n\t"     \
      "out  __SP_L__, r28          \n\t"     \
      "ld   r29, x+                \n\t"     \
      "out  __SP_H__, r29          \n\t");

#define FATAL_ERROR(msg)            \
  cli();                            \
  printf("FATAL ERROR: %s\n", msg); \
  for (;;)                          \
    ;

// void create_task(void (*fn)(void), uint8_t priority, Task_Queue* task_queue);
void init_empty_queue(volatile Task_Queue*);

// Must be run when multitasking is off
uint8_t add_task_to_queue(uint8_t, volatile Task_Queue*);

void create_task(void (*fn)(void), uint8_t, volatile Scheduler*);

#define SPAWN_TASK(fn, priority, schedule)                                     \
  {                                                                            \
    DISABLE_MT();                                                              \
    SAVE_SP();                                                                 \
    schedule->prev_task = schedule->queue.curr_task_index;                     \
    Simplos_Task* old_task = (Simplos_Task*)&schedule->queue                   \
                                 .task_queue[schedule->queue.curr_task_index]; \
    old_task->status = READY;                                                  \
    old_task->task_sp = task_sp;                                               \
    create_task(fn, priority, (volatile Scheduler*)schedule);                  \
    ENABLE_MT();                                                               \
  }

INLINED
void save_running_task(void) {
  INTERNAL_LED_PORT |= (1 << INTERNAL_LED);
  SAVE_CONTEXT();
  SAVE_SP();

  Simplos_Task* prev = (Simplos_Task*)&simplos_schedule.queue
                           .task_queue[simplos_schedule.queue.curr_task_index];
  prev->task_sp = task_sp;
  prev->status = READY;
}

INLINED
void prepare_next_task(Simplos_Task* task) {
  printf("Restoring context to task: %d\n", task->task_memory_block);
  INTERNAL_LED_PORT &= ~(1 << INTERNAL_LED);

  task->status = RUNNING;
  task_sp = task->task_sp;
  SET_SP();
  RESTORE_CONTEXT();
}

INLINED
void context_switch(void) {
  printf("Context switch!\n");
  save_running_task();

  Simplos_Task* new_task = (Simplos_Task*)select_next_task(
      &simplos_schedule);  // Updates curr_task_index
  prepare_next_task(new_task);
}

/*
  Yield current task.
*/
INLINED
void yield(void) {
  // "Reset" timer
  TCNT1 = 0;
  // Ensure MT is enabled.
  ENABLE_MT();
  // Call the interupt routine to simulate an "ordinary" fiering of the
  // interupt.
  context_switch();
}

#endif  // SIMPLOS_H_