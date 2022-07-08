#if !defined(SIMPLOS_H_)
#define SIMPLOS_H_

#include <avr/interrupt.h>
#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/atomic.h>

#include "io_helpers.h"
#include "memory.h"
#include "scheduler.h"
#include "serial.h"
#include "simplos_types.h"
#include "tasks.h"
#include "timers.h"

#define INLINED static inline __attribute__((always_inline))

#define INTERNAL_LED_PORT PORTB
#define INTERNAL_LED PORTB5
// clang-format off
#if defined(__clang__)
_Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"")
#endif
//clang-format on

extern volatile Kernel internal_kernel; 
extern Kernel volatile *volatile kernel; 

extern volatile uint16_t *volatile task_sp;
extern volatile uint16_t internal_task_sp_adr;

#define simplos_schedule (kernel->simplos_schedule)

#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)
//clang-format off
#if defined(__clang__)
#define CLANG_IGNORE_BEGIN(warning)                             \
  _Pragma("clang diagnostic push")                              \
  _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\"") \
  DO_PRAGMA(clang diagnostic ignored warning)
#define CLANG_IGNORE_END() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define GCC_IGNORE_BEGIN(warning)                             \
  _Pragma("GCC diagnostic push")                              \
  _Pragma("GCC diagnostic ignored \"-Wunknown-pragmas\"") \
  DO_PRAGMA(GCC diagnostic ignored warning)
#define GCC_IGNORE_END() _Pragma("GCC diagnostic pop")
#endif  // __GNUC__

#if defined(__clang__)
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING() \
  CLANG_IGNORE_BEGIN("-Wincompatible-pointer-types-discards-qualifiers")
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING() CLANG_IGNORE_END()
#elif defined(__GNUC__)
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING() \
  GCC_IGNORE_BEGIN("-Wdiscarded-qualifiers")
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING() GCC_IGNORE_END()
#endif  // __GNUC__

// clang-format on
/*
 * Add a task to the task queue. This is needed to let the the task execute.
 * */
uint8_t add_task_to_queue(uint8_t priority, Task_Queue *queue);

/*
 * Set up the scheduler.
 * */
void init_schedule(void);

/*
 * Initialise heap mapping. Sets free chunks to 0xFF (U8).
 */
void init_memory(void);

Simplos_Task *get_task(pid_type pid);
enum Task_Status task_status(pid_type pid);

void kill_current_task(void);

INLINED void reset_timer(void) { TCNT1 = 0; }

__attribute__((noinline, naked)) void k_yield(void);
__attribute__((noinline)) uint16_t spawn_task(void (*fn)(void),
                                              uint8_t const priority,
                                              char const *name);

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
      "lds   r26, task_sp           \n\t" \
      "lds   r27, task_sp +1        \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t" \
      "sei                          \n\t");

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
      "lds  r27, task_sp +1        \n\t" \
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

static inline __attribute__((always_inline, unused)) void context_switch(void) {
  SAVE_CONTEXT();
  // Use OS stack location
  SAVE_SP();
  SP = simplos_schedule.os_task_sp;

  {
#if defined(HW_TIME_MEASSUREMENTS)
    output_curr_task(OS_TASK_BLOCK);
#endif

    taskptr_type prev =
        &simplos_schedule.queue
             .task_queue[simplos_schedule.queue.curr_task_index];
#if defined(SW_TIME_MEASSREMENTS)
    // Increment CPU time counter for previous task
    prev->time_counter += GET_TICK_COUNTER();

    // TODO: improve this! Make the measurements more fair for task/os.
    // Reset counter to get an approximate value for time spent in interupt.
    RESET_TICK_COUNTER();
#endif  // SW_TIME_MEASSREMENTS
    if (prev->status == RUNNING) {
      // The previous task has been killed.
      prev->task_sp_adr = *task_sp;
      prev->status = READY;
#if defined(VERBOSE_OUTPUT)
      print_schedule();
#endif                               // VERBOSE_OUTPUT
      assert_stack_integrity(prev);  // FIXME put this check eralier so
                                     // errors will be easier to detect.
#if defined(VERBOSE_OUTPUT)
      cprint("saving task %d's SP 0x%X\n", prev->task_memory_block,
             prev->task_sp_adr);
#endif
    }
    select_next_task();
    taskptr_type task =
        &simplos_schedule.queue
             .task_queue[simplos_schedule.queue.curr_task_index];

#if defined(VERBOSE_OUTPUT)
    print_task(task, true);
#endif  // defined VERBOSE_OUTPUT
    task->status = RUNNING;
    *task_sp = task->task_sp_adr;
#if defined(VERBOSE_OUTPUT)
    cprint("Setting task_sp to 0x%X\n", *task_sp);
#endif  // defined VERBOSE_OUTPUT

#if defined(SW_TIME_MEASSREMENTS)
    // Add this context switch (approx.) to time counters.
    kernel->cs_time_counter += GET_TICK_COUNTER();
    // Reset timer and "start" counting the rest as task CPU time.
    RESET_TICK_COUNTER();
#endif  // SW_TIME_MEASSREMENTS

#if defined(HW_TIME_MEASSUREMENTS)
    output_curr_task(simplos_schedule->queue.curr_task_index);
#endif
  }
  reset_timer();
  SET_SP();
  RESTORE_CONTEXT();
  sei();
}

#endif  // SIMPLOS_H_
