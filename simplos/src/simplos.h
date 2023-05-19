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

extern volatile uint16_t task_sp;

extern volatile Kernel internal_kernel; 
extern Kernel volatile *volatile kernel; 


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

#define INDEX_OF_CURRENT_TASK \
  kernel->schedule.queue.task_index_queue[kernel->schedule.queue.queue_position]

/*
 * Add a task to the task queue. This is needed to let the the task execute.
 * */
uint8_t add_to_task_list(uint8_t priority, Task_Queue *queue);

/*
 * Set up kernel configuration.
 * */
void init_kernel(void);

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

#define STRINGIFY_WRAPPER(X) #X
#define AS_STRING(X) STRINGIFY_WRAPPER(X)

#define SAVE_CONTEXT()                    \
  asm volatile(                           \
      "push  r0                    \n\t"  \
      "in    r0, __SREG__          \n\t"  \
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
      "ldi   r26, 0x00              \n\t" \
      "ldi   r27, 0x21              \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t");

#define RESTORE_CONTEXT()                \
  asm volatile(                          \
      "ldi  r26, 0x00              \n\t" \
      "ldi  r27, 0x21              \n\t" \
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

#define SET_SP()                  \
  asm volatile(                   \
      "push r26             \n\t" \
      "push r27             \n\t" \
      "push r28             \n\t" \
      "push r29             \n\t" \
      "ldi  r26, 0x00       \n\t" \
      "ldi  r27, 0x21       \n\t" \
      "ld   r28, x+         \n\t" \
      "ld   r29, x+         \n\t" \
      "out  __SP_L__, r28   \n\t" \
      "out  __SP_H__, r29   \n\t");

#define SAVE_SP()                 \
  asm volatile(                   \
      "push r26             \n\t" \
      "push r27             \n\t" \
      "push r16             \n\t" \
      "push r17             \n\t" \
      "ldi  r26, 0x00       \n\t" \
      "ldi  r27, 0x21       \n\t" \
      "ldi  r17, 0x4        \n\t" \
      "in   r16, __SP_L__   \n\t" \
      "add  r16, r17        \n\t" \
      "st   x+, r16         \n\t" \
      "in   r16, __SP_H__  \n\t"  \
      "clr  r17             \n\t" \
      "adc  r16, r17        \n\t" \
      "st   x+, r16         \n\t" \
      "pop  r17             \n\t" \
      "pop  r16             \n\t" \
      "pop  r27             \n\t" \
      "pop  r26             \n\t");

#define SET_SP_TO_OS_TASK_SP()      \
  asm volatile(                     \
      "ldi r26, lo8(%0)       \n\t" \
      "ldi r27, hi8(%0)       \n\t" \
      "out __SP_L__, r26      \n\t" \
      "out __SP_H__, r27      \n\t" ::"i"(OS_STACK_START));

static inline __attribute__((always_inline, unused)) void context_switch(void) {
  SAVE_CONTEXT();
  SAVE_SP();
  SCILENT_DISABLE_MT();
  SET_SP_TO_OS_TASK_SP();

  taskptr_type prev = &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK];
  cprint("printing task:\n");
  print_task(prev);
  assert_task_pointer_integrity(prev);

  if (prev->status == RUNNING) {
    // The previous task has been killed.
    prev->task_sp_adr = task_sp;
    prev->status = READY;
#if defined(VERBOSE_OUTPUT)
    print_schedule();
#endif                                    // VERBOSE_OUTPUT
    assert_task_pointer_integrity(prev);  // FIXME put this check eralier so
                                          // errors will be easier to detect.
#if defined(VERBOSE_OUTPUT)
    cprint("saving task %d's SP 0x%X\n", prev->task_memory_block,
           prev->task_sp_adr);
#endif
  }
  select_next_task();
  taskptr_type task = &kernel->schedule.queue.tasks[INDEX_OF_CURRENT_TASK];

#if defined(VERBOSE_OUTPUT)
  cprint("printing task:\n");
  print_task(task);
  assert_task_pointer_integrity(task);
#endif  // defined VERBOSE_OUTPUT
  task->status = RUNNING;
  task_sp = task->task_sp_adr;
#if defined(VERBOSE_OUTPUT)
  cprint("Setting task_sp to 0x%X\n", task_sp);
#endif  // defined VERBOSE_OUTPUT

  RESET_TIMER();
  SET_SP();
  RESTORE_CONTEXT();
  SCILENT_ENABLE_MT();
}

typedef uint8_t Index;
void set_task_name(Index task_index, const char *name);
Index create_simplos_task(const char *name, const uint8_t priority);

#endif  // SIMPLOS_H_
