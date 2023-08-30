#if !defined(SIMPLOS_H_)
#define SIMPLOS_H_

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
extern volatile uint16_t next_task_sp;
extern volatile uint16_t scheduler_task_sp;


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
#else // other tools like cppcheck
#define BEGIN_DISCARD_VOLATILE_QUALIFIER_WARNING()
#define END_DISCARD_VOLATILE_QUALIFIER_WARNING()
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
 * Initialise heap mapping and canaries.
 */
void init_memory(void);

uint16_t num_context_switch_overhead_bytes(void);

Simplos_Task *get_task(pid_type pid);
enum Task_Status task_status(pid_type pid);

void kill_current_task(void);

#define SAVE_CONTEXT()                    \
  asm volatile(                           \
      "push  r0                     \n\t" \
      "in    r0, __SREG__           \n\t" \
      "push  r0                     \n\t" \
      "push  r1                     \n\t" \
      "clr   r1                     \n\t" \
      "push  r2                     \n\t" \
      "push  r3                     \n\t" \
      "push  r4                     \n\t" \
      "push  r5                     \n\t" \
      "push  r6                     \n\t" \
      "push  r7                     \n\t" \
      "push  r8                     \n\t" \
      "push  r9                     \n\t" \
      "push  r10                    \n\t" \
      "push  r11                    \n\t" \
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
      "ldi   r26, lo8(%0)           \n\t" \
      "ldi   r27, hi8(%0)           \n\t" \
      "in    r0, __SP_L__           \n\t" \
      "st    x+, r0                 \n\t" \
      "in    r0, __SP_H__           \n\t" \
      "st    x+, r0                 \n\t" \
      : /* No outputs*/                   \
      : "i"(&task_sp));

#define SAVE_SP()                 \
  asm volatile(                   \
      "ldi  r26, lo8(%0)    \n\t" \
      "ldi  r27, hi8(%0)    \n\t" \
      "in   r16, __SP_L__   \n\t" \
      "st   x+, r16         \n\t" \
      "in   r16, __SP_H__   \n\t" \
      "st   x+, r16         \n\t" \
      : /* No outputs*/           \
      : "i"(&task_sp));

#define SELECT_SCHEDULED_TASK_OR_SCHEDULER()                   \
  asm volatile(                                                \
      "ldi r26, lo8(%[next_sp])         \n\t"                  \
      "ldi r27, hi8(%[next_sp])         \n\t"                  \
      "ldi r28, lo8(%[task_sp])         \n\t"                  \
      "ldi r29, hi8(%[task_sp])         \n\t"                  \
      "ld  r16, x+                      \n\t"                  \
      "ld  r17, x                       \n\t"                  \
      "mov r19, r16                     \n\t"                  \
      "or  r19, r17                     \n\t"                  \
      "cpse r19, __zero_reg__           \n\t"                  \
      "rjmp .+8                         \n\t"                  \
      "ldi r30, lo8(%[scheduler_sp])    \n\t"                  \
      "ldi r31, hi8(%[scheduler_sp])    \n\t"                  \
      "ld  r16, z+                      \n\t"                  \
      "ld  r17, z                       \n\t"                  \
      "st  y+, r16                      \n\t"                  \
      "st  y,  r17                      \n\t"                  \
      "st  x, __zero_reg__              \n\t"                  \
      "st  -x, __zero_reg__             \n\t"                  \
      : /* No outputs */                                       \
      : [next_sp] "i"(&next_task_sp), [task_sp] "i"(&task_sp), \
        [scheduler_sp] "i"(&scheduler_task_sp));

#define SET_SP()                  \
  asm volatile(                   \
      "ldi  r26, lo8(%0)    \n\t" \
      "ldi  r27, hi8(%0)    \n\t" \
      "ld   r28, x+         \n\t" \
      "ld   r29, x+         \n\t" \
      "out  __SP_L__, r28   \n\t" \
      "out  __SP_H__, r29   \n\t" \
      : /*No outputs*/            \
      : "i"(&task_sp));

#define RESTORE_CONTEXT()                \
  asm volatile(                          \
      "ldi  r26, lo8(%0)           \n\t" \
      "ldi  r27, hi8(%0)           \n\t" \
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
      "pop  r0                     \n\t" \
      : /*No outputs*/                   \
      : "i"(&task_sp));

#define CONTEXT_SWTICH()                \
  SAVE_CONTEXT();                       \
  SAVE_SP();                            \
  SCILENT_DISABLE_MT();                 \
  SELECT_SCHEDULED_TASK_OR_SCHEDULER(); \
  RESET_TIMER();                        \
  SET_SP();                             \
  RESTORE_CONTEXT();                    \
  SCILENT_ENABLE_MT();

typedef uint8_t Index;
void set_task_name(Index task_index, const char *name);
Index create_simplos_task(const char *name, const uint8_t priority);

#endif  // SIMPLOS_H_
