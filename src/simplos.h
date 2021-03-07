#ifndef SIMPLOS_H_
#define SIMPLOS_H_

// #define  STACK_POINTER ((volatile uint16_t * const) 0x5E)

#include "simplos_types.h"
#include "tasks.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define ENABLE_MT()         \
  TIMSK1 |= (1 << OCIE2A);  \
  sei();                    \
  printf("enabling MT\n");

#define DISABLE_MT()        \
 TIMSK1 &= ~(1 << OCIE2A);  \
 printf("disabling MT\n"); 

extern volatile uint16_t task_sp;

#define force_inline static inline  __attribute__((always_inline))

// Somewhat borrowed from 
// https://www.freertos.org/kernel/secondarydocs.html

// TODO use STACK_POINTER macro instead of asm.
#define SAVE_CONTEXT()              \
asm volatile (                      \
  "push  r0                    \n\t"\
  "in    r0, __SREG__          \n\t"\
  "cli                         \n\t"\
  "push  r0                    \n\t"\
  "push  r1                    \n\t"\
  "clr   r1                    \n\t"\
  "push  r2                    \n\t"\
  "push  r3                    \n\t"\
  "push  r4                    \n\t"\
  "push  r5                    \n\t"\
  "push  r6                    \n\t"\
  "push  r7                    \n\t"\
  "push  r8                    \n\t"\
  "push  r9                    \n\t"\
  "push  r10                   \n\t"\
  "push  r11                   \n\t"\
  "push  r12                    \n\t"\
  "push  r13                    \n\t"\
  "push  r14                    \n\t"\
  "push  r15                    \n\t"\
  "push  r16                    \n\t"\
  "push  r17                    \n\t"\
  "push  r18                    \n\t"\
  "push  r19                    \n\t"\
  "push  r20                    \n\t"\
  "push  r21                    \n\t"\
  "push  r22                    \n\t"\
  "push  r23                    \n\t"\
  "push  r24                    \n\t"\
  "push  r25                    \n\t"\
  "push  r26                    \n\t"\
  "push  r27                    \n\t"\
  "push  r28                    \n\t"\
  "push  r29                    \n\t"\
  "push  r30                    \n\t"\
  "push  r31                    \n\t"\
);

#define SAVE_SP()                 \
asm volatile (                       \
  "lds   r26, task_sp               \n\t"\
  "lds   r27, task_sp + 1           \n\t"\
  "in    r0, __SP_L__           \n\t"\
  "st    x+, r0                 \n\t"\
  "in    r0, __SP_H__           \n\t"\
  "st    x+, r0                 \n\t" \
);

#define RESTORE_CONTEXT()             \
asm volatile (	                      \
  "pop  r31                    \n\t"  \
  "pop  r30                    \n\t"  \
  "pop  r29                    \n\t"  \
  "pop  r28                    \n\t"  \
  "pop  r27                    \n\t"  \
  "pop  r26                    \n\t"  \
  "pop  r25                    \n\t"  \
  "pop  r24                    \n\t"  \
  "pop  r23                    \n\t"  \
  "pop  r22                    \n\t"  \
  "pop  r21                    \n\t"  \
  "pop  r20                    \n\t"  \
  "pop  r19                    \n\t"  \
  "pop  r18                    \n\t"  \
  "pop  r17                    \n\t"  \
  "pop  r16                    \n\t"  \
  "pop  r15                    \n\t"  \
  "pop  r14                    \n\t"  \
  "pop  r13                    \n\t"  \
  "pop  r12                    \n\t"  \
  "pop  r11                    \n\t"  \
  "pop  r10                    \n\t"  \
  "pop  r9                     \n\t"  \
  "pop  r8                     \n\t"  \
  "pop  r7                     \n\t"  \
  "pop  r6                     \n\t"  \
  "pop  r5                     \n\t"  \
  "pop  r4                     \n\t"  \
  "pop  r3                     \n\t"  \
  "pop  r2                     \n\t"  \
  "pop  r1                     \n\t"  \
  "pop  r0                     \n\t"  \
  "out  __SREG__, r0           \n\t"  \
  "pop  r0                     \n\t"  \
);

#define SET_SP()                  \
asm volatile(                        \
  "lds  r26, task_sp               \n\t" \
  "lds  r27, task_sp + 1           \n\t" \
  "ld   r28, x+                \n\t" \
  "out  __SP_L__, r28          \n\t" \
  "ld   r29, x+                \n\t" \
  "out  __SP_H__, r29          \n\t" \
);

// void create_task(void (*fn)(void), uint8_t priority, Task_Queue* task_queue);
void init_empty_queue(Task_Queue*);

// Must be run when multitasking is off
uint8_t add_task_to_queue(uint8_t, Task_Queue*);

void create_task(void (*fn)(void), uint8_t, volatile Scheduler*);


force_inline
void change_task_sp(uint8_t task_index,  Scheduler * schedule)
{
  task_sp = schedule->queue.task_queue[task_index].task_sp;
  SET_SP();
}

force_inline
void spawn_task(void (*fn)(void), uint8_t priority, Scheduler* schedule)
{
  SAVE_CONTEXT();
  SAVE_SP();
  create_task(fn, priority, (volatile Scheduler *)schedule);
}

/*
  Yield current task.
*/
force_inline
void yield()
{
  // "Reset" timer
  TCNT1 = 0;
  // Ensure MT is enabled.
  ENABLE_MT();
  // Call the interupt routine to simulate an "ordinary" fiering of the interupt.
  TIMER1_COMPA_vect();
}

/*
// Starts the multitasking by enabling timer interupts and starting the idle task.
#define ENABLE_MT_IDLE_TASK(first_task)         \
  cli();                                        \
  printf("Initiating first task!!!!\n");        \
  first_task->status = READY;                   \
  sei();                                        \
  first_task->fn();
*/

#define FATAL_ERROR(msg)              \
  cli();                              \
  printf("FATAL ERROR: %s\n", msg);   \
  for(;;);

// Always inlined
force_inline
void setup_idle_task(void (*fn)(Scheduler* schedule), Scheduler* schedule)
{
  DISABLE_MT();
  // SAVE_CONTEXT();

  uint8_t index = add_task_to_queue(0, &schedule->queue);
  Simplos_Task* new_task = &schedule->queue.task_queue[index];
  new_task->status = READY;
  schedule->queue.curr_task_index = index;
  schedule->force_prev = false;
  task_sp = new_task->task_sp;
  SET_SP();
  ENABLE_MT();
  fn(schedule);
}

static inline void idle_fn(Scheduler* schedule)
{
  printf("In idle loop!\n");

  // main_sp = *STACK_POINTER;
  spawn_task(test_fn1, 1, schedule);

  // // main_sp = *STACK_POINTER;
  spawn_task(test_fn2, 1, schedule);

  spawn_task(test_fn3, 1, schedule);

  for(;;) yield();

  // Finally let the scheduler run!
  for (;;)
  {
    for (uint16_t i = 0; i < 0xFFFF; ++i) { ; }
    printf(".\n");
    fflush(stdout);
  }
}

#endif // SIMPLOS_H_